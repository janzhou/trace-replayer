#if !defined(_GNU_SOURCE)
#	define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include	<pthread.h>
#include	<time.h>
#include <libaio.h>
#include <semaphore.h>

#include <errno.h>
#include <sys/stat.h>


#include <pthread.h>
#include <asm/types.h>
#include <sys/types.h>

#include "replayer.h"
#include "feedback.h"

pthread_t feedback_tid;

__u64 num_disk_access[NUM_DISK] = {0}, num_layout_access[LAYOUT_MAX] = {0}, total_access = 0;

inline int sleep_disk(disk_t* disk)
{
	char cmd1[50],cmd2[50];
	if(feedback & REAL_SLEEP_ON){
		close(disk->dev_id);
		disk->dev_id = 0;
		sprintf(cmd1, "hdparm -S 1  %s", disk->dev_name);// -B 1
		sprintf(cmd2, "hdparm -Y %s", disk->dev_name);
		printf("sleep disk %s\n", disk->dev_name);
		return system(cmd1)||system(cmd2);;
	}
	else  return -1;
}

inline int wakeup_disk(disk_t* disk)
{
	char cmd[50];
	if(feedback & REAL_SLEEP_ON){
		sprintf(cmd, "hdparm -S 253 %s", disk->dev_name);//-B 255 
		printf("wakeup disk %s\n", disk->dev_name);
		return system(cmd)||(!(disk->dev_id = open(disk->dev_name, O_RDWR | O_DIRECT | O_NOATIME)));
	}
	else return -1;
}

static inline void caculate_access_rate(void)
{
	static __u64 num_disk_access_old[NUM_DISK] = {0}, num_layout_access_old[LAYOUT_MAX] = {0}, num_buf;
	static int disk_status_pre[NUM_DISK]={0};
	int i,j;
	
	total_access = 0;
	for(i=0; i< LAYOUT_MAX; i++){
		num_buf = layouts[i].num_access;
		num_layout_access[i] = num_buf - num_layout_access_old[i];
		num_layout_access_old[i] = num_buf;
	}
	for(i=0; i<ndisk; i++){
		num_buf = disks[i].num_access;
		num_disk_access[i] = num_buf - num_disk_access_old[i];
		num_disk_access_old[i] = num_buf;
		if(num_disk_access[i] != 0){//有访问
			if(disks[i].status <= 0) disks[i].status = 1;
			else disks[i].status++;
			disk_status_pre[i]=0;
		}
		else {
			for(j=0; j<LAYOUT_MAX; j++){
				if(layouts[j].disk_map == i) break;
			}
			if(j < LAYOUT_MAX){//没有访问但是有映射，说明仍然为on
				if(disks[i].status <= 0) disks[i].status = 1;
				else disks[i].status++;
				disk_status_pre[i]=0;
			}else{//没有访问，没有映射,切换至off或者on至off的中间状态
				if(disks[i].status == 0){//如果状态为on至off的中间状态
					if(disk_status_pre[i]*interval < 120){//中间状态至少保持2分钟
						if(0 == sleep_disk(disks+i)){
							disks[i].status = -1;
						}
					}else disk_status_pre[i]++;
				}
				else if(disks[i].status > 0){
					disks[i].status = 0;
				}else if(disks[i].status < 0){
					disks[i].status--;
				}
			}
		}
		fprintf(thelog,"%Lf\t%f\t%f\t",\
			(long double)num_disk_access[i]/(long double)interval,disks[i].temperature,disks[i].t0);
			total_access += num_disk_access[i];
		disk_status_pre[i] = disks[i].status;
	}
	fprintf(thelog,"%Lf\n", (long double)total_access/(long double)interval);
	printf("caculate_access_rate %lld\n", total_access);
}

static int disk_status[NUM_DISK];
static __u64 average_ios;
static inline void select_disk(void)
{
	int i,j,k;

	int disk_need,disk_select;
	
	if(!(feedback & DISK_SLEEP_ON)){
		average_ios = total_access/ndisk;
		return;
	}
	
	disk_select = disk_need = total_access / (MAX_IOPS*interval) + 1;

	printf("disk need %d | ", disk_need);

	if(feedback&TEMPERATURE_FEEDBACK_ON){
		for(i = 0; i < ndisk; i++){
			if(disks[i].status > 0 && not_red(disks[i].temperature)){//close red disk
				disk_status[i] = DISK_ON;
				disk_need--;
			}else{
				disk_status[i] = DISK_OFF;
			}
			printf("-%d-",disk_status[i]);
		}
		printf(" | %d | ",disk_need);
		while(disk_need > 0){//active sleeping disk
			for(i = 0, j = -1; i < ndisk; i++){
				if(disk_status[i] == DISK_OFF && is_green(disks[i].temperature)){//only active green disk
					if(j==-1) j=i;
					else if(disks[i].t0 < disks[j].t0) j = i;
				}
			}
			if(j!=-1 && disks[j].status * interval < -600){
				wakeup_disk(disks+j);
				disk_status[j] = DISK_ON;
				disk_need--;
				printf(" +%d ",j);
			}else break;//no more disk can switch to active
		}
		while(disk_need < 0){//close active disk, select yellow one
			for(i = 0, j = -1, k = 0; i < ndisk; i++){
				if(disk_status[i] == DISK_ON) k++;
				if(disk_status[i] == DISK_ON && is_yellow(disks[i].temperature)){
					if(j == -1) j = i;
					else if(disks[i].t0 > disks[j].t0) j = i;
				}
			}
			if(j != -1 && k > 1){//at least one disk need to be active
				disk_status[j] = DISK_OFF;
				disk_need++;
				printf(" -%d ",j);
			}else break;//no yellow disk close, break
		}
		while(disk_need<0){//close active disk, select green one
			for(i = 0, j = -1, k = 0; i < ndisk; i++){
				if(disk_status[i] == DISK_ON) k++;
				if(disk_status[i] == DISK_ON&&is_green(disks[i].temperature)){
					if(j == -1) j = i;
					else if(disks[i].t0 > disks[j].t0) j = i;
				}
			}
			if(j != -1 && k > 1){//at least one disk need to be active
				disk_status[j] = DISK_OFF;
				disk_need++;
				printf(" -%d ",j);
			}else break;//no green disk close, break
		}
		for(i = 0; i < ndisk; i++){//switch active yellow to sleep and sleeping green to active
			if(disk_status[i] == DISK_ON && is_yellow(disks[i].temperature)){//yellow active
				for(k = 0, j = -1; k < ndisk; k++){
					if(disk_status[k] == DISK_OFF && is_green(disks[k].temperature)){//green sleep
						if(j==-1) j=k;
						else if(disks[k].t0 < disks[j].t0) j = i;
					}
				}
				if(j!=-1 && disks[j].status * interval < -180){//sleep at least 3m
					wakeup_disk(disks+j);
					disk_status[j] = DISK_ON;
					disk_status[i] = DISK_OFF;
					printf(" +%d -%d ",j, i);
				}else break;
			}
		}
		goto select_end;
	}
//-------------------------------------无温度反馈
	for(i = 0; i < ndisk; i++){
		if(disks[i].status > 0){
			disk_status[i] = DISK_ON;
			disk_need--;
		}else{
			disk_status[i] = DISK_OFF;
		}
		printf("-%d-",disk_status[i]);
	}
	printf(" %d",disk_need);
	while(disk_need > 0){//硬盘少了，打开睡眠时间最长的，至少已睡10分钟
		for(i = 0, j = -1; i < ndisk; i++){
			if(disk_status[i] == DISK_OFF  && disks[i].status < 0){//状态为0是从on至off的中间状态，不可用
				if(j==-1) j=i;
				else if(disks[i].status < disks[j].status) j = i;
			}
		}
		if(j!=-1 && disks[j].status * interval < -600){
			wakeup_disk(disks+j);
			disk_status[j] = DISK_ON;
			disk_need--;
			printf(" +%d ",j);
		}else break;
	}
	while(disk_need < 0){//硬盘多了，关工作时间最长的
		for(i = 0, j = -1, k = 0; i < ndisk; i++){
			if(disk_status[i] == DISK_ON){
				k++;
				if(j == -1) j = i;
				else if(disks[i].status > disks[j].status) j = i;
			}
		}
		if(j != -1 && k > 1){// && disks[j].status * interval > 600
			disk_status[j] = DISK_OFF;
			disk_need++;
			printf(" -%d ",j);
		}else break;
	}
select_end:
	printf(" | ");
	disk_select -= disk_need;
	average_ios = total_access/disk_select;
	for(i = 0; i < ndisk; i++){
		printf("-%d-", disk_status[i]);
	}
	printf("\n");
}

static int ordered_layout[LAYOUT_MAX];
static int ordered_disk[NUM_DISK];
int order(int *order, __u64 * target, int num)
{
	int i, j, k, ret = 0;
	do{
		k = 0;
		for(i = 0; i < num-1; i++){
			if(*(target + *(order+i)) < *(target + *(order+i+1))){
				j = *(target + *(order+i));
				*(target + *(order+i)) = *(target + *(order+i+1));
				*(target + *(order+i+1)) = j;
				k++;
			}
		}
		ret++;
	}while(k);
	return ret;
}

static inline void layout_transfer(int target, __u64 layout)
{
	num_disk_access[layouts[layout].disk_map] -= num_layout_access[layout];
	num_disk_access[target] += num_layout_access[layout];
	layouts[layout].disk_map = target;
}

static inline void change_layout(void)
{
	int i, j, k;
	printf("change layout\n");
	for(i=0;i<LAYOUT_MAX ;i++){
		if(disk_status[layouts[i].disk_map] == DISK_OFF){
			for(j=0, k=-1;j<ndisk;j++){
				if(disk_status[j] == DISK_ON){
					if(k == -1) k = j;
					else if(num_disk_access[j] < num_disk_access[k])
						k = j;
				}
			}
			if(k!=-1)layout_transfer(k, i);
		}
	}

	order(ordered_layout, num_layout_access, LAYOUT_MAX);

//	for(k=0;k<LAYOUT_MAX ;k++){
//		i = ordered_layout[k];
	for(i=0;i<LAYOUT_MAX ;i++){
		if(num_layout_access[i] == 0) continue;
		for(j=0;j<ndisk;j++){
			if(disk_status[j] == DISK_OFF) continue;
			if(layouts[i].disk_map == j) continue;
			__u64 a = num_disk_access[layouts[i].disk_map];
			__u64 b = num_disk_access[layouts[i].disk_map] - num_layout_access[i];
			__u64 c = num_disk_access[j];
			__u64 d = num_disk_access[j] + num_layout_access[i];
//			printf("acbd %d-%lld %d-%lld %lld %lld %lld",layouts[i].disk_map,a,j,c,b,d,num_layout_access[i]);
			if(a > average_ios) a = a - average_ios;else a = average_ios -a;
			if(b > average_ios) b = b - average_ios;else b = average_ios -b;
			if(c > average_ios) c = c - average_ios;else c = average_ios -c;
			if(d > average_ios) d = d - average_ios;else d = average_ios -d;
			if((a+c)>(b+d)){
				layout_transfer(j, i);
//				printf("  go %lld %lld\n",a+c,b+d);
//				printf("%d-%d;",j,i);
				break;
			}
//			printf("\n");
		}
	}
	for(j=0;j<ndisk;j++){
		printf("%lld\t",num_disk_access[j]);
	}printf("%lld\n", average_ios);

}

static void *feedback_th(void *arg)
{
	int i;
	sleep(1);

	for(i=0; i<ndisk; i++){
		fprintf(thelog,"iops_%d\tt_%d\tt0_%d\t",i,i,i);
	}fprintf(thelog,"iops_total\n");
	
	
	printf("feedback start %d %d\n", feedback, interval);
	sleep(interval-1);

	while(1){
		caculate_access_rate();
		if(feedback & PERFORMANCE_FEEDBACK_ON) select_disk();
		if(feedback & PERFORMANCE_FEEDBACK_ON) change_layout();
		fflush(thelog);fflush(stdout);
		sleep(interval);
	}
	return arg;
}
static void __attribute__((constructor)) feedback_init(void)
{
	int i;
	for(i=0; i < LAYOUT_MAX; i++){
		ordered_layout[i] = i;
	}
	for(i=0; i< NUM_DISK; i++){
		ordered_disk[i] = i;
	}

	if (pthread_create(&feedback_tid, NULL, feedback_th, NULL) != 0) {
		printf("create feedback thread error\n");
		exit(0);
	};

	printf("feedback init end\n");
}

