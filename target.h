#ifndef TARGET_H
#define TARGET_H
#include <linux/sched/types.h>
#include <linux/types.h>

#define PROCESS_DATA _IOWR('a', 'a', struct target_t*)

struct task_cputime_t {
	uint64_t stime;
	uint64_t utime;
	uint64_t sum_exec_runtime;
};

struct net_device_t {
	uint32_t mem_start;
	uint32_t mem_end;
	uint64_t base_address;
	uint64_t state;
	unsigned char MAC;
};

struct target_t {
	struct task_cputime_t* ctime;
	struct net_device_t* dnet;
};

struct incoming_data {
	int32_t* pid;
	char* dev_name;
	struct target_t* res;
};

#endif