#include <asm/types.h>

__u64 gettime(void);
void myNanoSleep(__u64 stime, __u64 etime, __u64 gtime);
int stall(__u64 tclock, __u64 oclock);
