#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pid.h>
#include <linux/path.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include "target.h"
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MIKHAIL");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define MYMAJOR 300

int32_t pid = 0;
char dname[1000] = {0};
struct net_device_t dnet;


static void set_dnet(void) {
	struct net_device* dev;
	dev = dev_get_by_name(&init_net, dname);
	if(dev == NULL) {
		pr_err("Cannot get device with name : %s", dname);
	} else {
		pr_info("Collect net_device");
		unsigned char dev_addr;
		if(dev->dev_addr == NULL) {
			dev_addr = 0;
		} else {
			dev_addr = dev->dev_addr;
		}
		dnet = (struct net_device_t) {
			.mem_start = dev->mem_start,
			.mem_end = dev->mem_end,
			.base_address = dev->base_addr,
			.state = dev->state,
			.MAC = dev_addr
		};
	}
}

static struct task_cputime_t calc_ctime(void) {
	pr_info("Collecting cpu-time for %ld", pid);

	struct task_struct* task;
	task = get_pid_task(find_get_pid(pid), PIDTYPE_PID);

	struct task_cputime_t ctime = {task->stime, task->utime, task->stime + task->utime};
	
	return ctime;
}

// This function will be called when we read the Device file
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off){
	pr_info("Read Function\n");
	return 0;
}

// This function will be called when we write the Device file
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off){
	pr_info("Write function\n");
	return len;
}

static int etx_open(struct inode *inode, struct file *file){
	pr_info("Device File Opened\n");
	return 0;
}

// This function will be called when we write IOCTL on the Device file
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	struct task_cputime_t res;
	switch(cmd) {
		case WR_PID:
			if(copy_from_user(&pid ,(int*) arg, sizeof(pid))) {
				pr_err("Data Write : Err!\n");
			}
			pr_info("Pid = %d\n", pid);
			break;
		case WR_DNAME:
			if(copy_from_user(dname ,(char*) arg, strlen(arg))) {
				pr_err("Data Write : Err!\n");
			}
			dname[strlen(arg)] = '\0';
			pr_info("Device name = %s\n", dname);
			set_dnet();
			break;
		case RD_CTIME:
			res = calc_ctime();
			if( copy_to_user((struct task_cputime_t*) arg, &res, sizeof(struct task_cputime_t)) ){
				pr_err("Data Read : Err!\n");
			}
			break;
		case RD_DNET:
			if( copy_to_user((struct net_device_t*) arg, &dnet, sizeof(struct net_device_t)) ){
				pr_err("Data Read : Err!\n");
			}
			break;
		default:
			pr_info("Command not found\n");
			break;
	}
	return 0;
}


// This function will be called when we close the Device file
static int etx_release(struct inode *inode, struct file *file){
        pr_info("Device File Closed\n");
        return 0;
}

static struct file_operations fops ={
	.owner          = THIS_MODULE,
	.read           = etx_read,
	.write          = etx_write,
	.open           = etx_open,
	.unlocked_ioctl = etx_ioctl,
	.release        = etx_release,
};

static int __init lkm_example_init(void) {
	printk(KERN_INFO "ioctl_mod: module loaded.\n");
	int retval;
	retval = register_chrdev(MYMAJOR, "etc_driver", &fops);
	if (0 == retval) {
		printk("et_driver device number Major:%d , Minor:%d\n", MYMAJOR, 0);
	} else if (retval > 0) {
		printk("etc_driver device number Major:%d , Minor:%d\n", retval >> 20, retval & 0xffff);
	} else {
		printk("Couldn't register device number!\n");
		return -1;
	}
	
	return 0;
}

static void __exit lkm_example_exit(void) {
	unregister_chrdev(MYMAJOR, "etc_driver");
	pr_info("Device Driver Removed\n");
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);
