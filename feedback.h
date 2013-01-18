
#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <stdio.h>

#define MAX_IOPS 400

#define MYABS(x) x>0?(x):(-(x))

#define DISK_ON 0x0001
#define DISK_OFF 0x0000

#define PERFORMANCE_FEEDBACK_ON 0x01
#define DISK_SLEEP_ON 0x02
#define REAL_SLEEP_ON 0x04
#define TEMPERATURE_FEEDBACK_ON 0x08
#define TEMPERATURE_PREDICT_ON 0x10

#define is_green(x) ((x)<26)
#define is_red(x) ((x)>30)
#define is_yellow(x) ((x)>26&&(x)<30)
#define not_red(x) ((x)<30)


int feedback;
int interval;

FILE* thelog;

#endif

