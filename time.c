#include <sys/time.h>
#include <time.h>
#include "time.h"

/**
 * ts2ns - Convert timespec values to a nanosecond value
 */
#define NS_TICKS		((__u64)1000 * (__u64)1000 * (__u64)1000)
static inline __u64 ts2ns(struct timespec *ts)
{
	return ((__u64)(ts->tv_sec) * NS_TICKS) + (__u64)(ts->tv_nsec);
}

/**
 * ts2ns - Convert timeval values to a nanosecond value
 */
static inline __u64 tv2ns(struct timeval *tp)
{
	return ((__u64)(tp->tv_sec)) + ((__u64)(tp->tv_usec) * (__u64)1000);
}

/**
 * gettime - Returns current time 
 */
 __u64 gettime(void)
{
	static int use_clock_gettime = -1;		// Which clock to use

	if (use_clock_gettime < 0) {
		use_clock_gettime = clock_getres(CLOCK_MONOTONIC, NULL) == 0;
		if (use_clock_gettime) {
			struct timespec ts = {
				.tv_sec = 0,
				.tv_nsec = 0
			};
			clock_settime(CLOCK_MONOTONIC, &ts);
		}
	}

	if (use_clock_gettime) {
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return ts2ns(&ts);
	}
	else {
		struct timeval tp;
		gettimeofday(&tp, NULL);
		return tv2ns(&tp);
	}
}

void myNanoSleep(__u64 stime, __u64 etime, __u64 gtime)
{	
	long long	gap;
	long long gap1;
	
	gap 	= stime+ gtime- etime;
	gap1 = etime - stime;

	if(stime + gtime < etime){
		return ;
	}

	struct timespec tv;
	tv.tv_sec = gap/(1000 * 1000 * 1000);
	tv.tv_nsec = gap%(1000 * 1000 * 1000);

	nanosleep(&tv,NULL);
	return;
}

int stall(__u64 tclock, __u64 oclock){	
	__u64 dreal;
	if(tclock< oclock){
		dreal = oclock - tclock;
		myNanoSleep(0, 0, dreal);
	}		
	
	return 1;
}