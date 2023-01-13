#ifndef TARGET_H
#define TARGET_H
#include <linux/sched/types.h>
#include <linux/types.h>

#define WR_PID _IOW('a','a', int32_t*)
#define WR_DNAME _IOW('a', 'b', char**)
#define RD_CTIME _IOR('a','c', struct task_cputime_t*)
#define RD_DNET _IOR('a','b', struct net_device_t*)

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

#endif