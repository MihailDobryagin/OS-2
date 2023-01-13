#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <inttypes.h>

#include "target.h"

static void print_dnet(struct net_device_t dnet) {
	printf("device memory start: %" PRIu32 "\n", dnet.mem_start);
	printf("device memory end: %" PRIu32 "\n", dnet.mem_end);
	printf("device I/O address: %" PRIu64 "\n", dnet.base_address);
	printf("device state: %" PRIu64 "\n", dnet.state);
	printf("device MAC: %pMF\n", &dnet.MAC);
}

int main(int argc, char *argv[]) {
	int fd;
	struct task_cputime_t ctime;
	struct net_device_t dnet;
	int32_t pid = atoi(argv[1]);
	char* dname = argv[2];
	
	if(argc < 2) {
		printf("The program needs an argument - a pid!\n");
		return -1;
	}
	if (pid < 1) {
		printf("Pid can be only greater than 0\n");
		return -1;
	}
	printf("\nOpening a driver...\n");
	fd = open("/dev/etc_device", O_WRONLY);
	if(fd < 0) {
		printf("Cannot open device file:(\n");
		return 0;
	}
	
	printf("Writing Pid to Driver\n");
    ioctl(fd, WR_PID, &pid);
    printf("Reading Result from Driver\n");
    ioctl(fd, RD_CTIME, &ctime);
    printf("Task_cputime:\n");
    printf("Stime: %" PRIu64 "\n", ctime.stime);
    printf("Utime: %" PRIu64 "\n", ctime.utime);
    printf("Sum_exec_runtime: %" PRIu64 "\n", ctime.sum_exec_runtime);
    printf("________________________\n");
    
	printf("Writing device name to Driver\n");
    ioctl(fd, WR_DNAME, dname);
	printf("Reading net device info from Driver\n");
	ioctl(fd, RD_DNET, &dnet);
	print_dnet(dnet);
	printf("________________________\n");
	
	close(fd);
	printf("Closing Driver\n");
	
	return 0;
}