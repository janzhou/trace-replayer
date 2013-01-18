/*
 *======================================================================
 *====  	create by rayth at 2011.6.10   =======================================
 *======================================================================
*/
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

#include "trace.h"
#include "time.h"
#include "replayer.h"

#define get_trace(a) (*(trace_engines + trace_type)->get_trace)(trace_file,a)
#define open_trace(a) (*(trace_engines + trace_type)->open_trace)(a)

io_context_t io_ctx;
int trace_type, trace_file;
iotrace trace[MAX_QUERY_LEN];
struct iocb ios[MAX_QUERY_LEN];
sem_t submit_finished;

//#define LOCK_FREE_REPLAY

#ifndef LOCK_FREE_REPLAY
	#include "list/list.h"
	list_head * iocb_list;
#else
	struct iocb* iocb_list[MAX_QUERY_LEN+1];
	int free_head, free_tail;
#endif

pthread_t submit_tid;
pthread_t receive_tid;
//====================================================================================================================
//submit
//====================================================================================================================
static void *submit_th(void* arg)
{
	iotrace* trace;
	struct iocb * io;
	__u64 layout_id;
	__u64 sector;
	int disk_id;
	void* buf;

	__u64 genesis_timeStamp = 0;
	__u64 genesisTrace_timeStamp = 0;
	int pagesize = getpagesize();
#ifndef LOCK_FREE_REPLAY
	io = (struct iocb*)get_from_list(iocb_list);
	trace = io->data;
#else
	while(free_head == free_tail) ;
	io = iocb_list[free_head];
	trace = io->data;
	if(free_head == MAX_QUERY_LEN - 1) free_head = 0;
	else free_head++;
#endif

	if(!get_trace(trace)){
		printf("end of trace file\n");
		sem_post(&submit_finished);
		return NULL;
	}
	genesis_timeStamp = gettime();
	genesisTrace_timeStamp = trace->time_stamp;
	goto io_submit;
next_io:
#ifndef LOCK_FREE_REPLAY
	io = (struct iocb*)get_from_list(iocb_list);
	trace = io->data;
#else
	while(free_head == free_tail) ;
	io = iocb_list[free_head];
	trace = io->data;
	if(free_head == MAX_QUERY_LEN - 1) free_head = 0;
	else free_head++;
#endif
trace_error:
	if(!get_trace(trace)){
	//	goto trace_error;
		printf("\nend of trace file\n");
		sem_post(&submit_finished);
		return arg;
	}
io_submit:
	layout_id = trace->sector/LAYOUT_BLOCK;
	if(layout_id > LAYOUT_MAX-1){
		printf("layout error------------------\n");
		goto trace_error;
	}
	trace->subsector = sector = layouts[layout_id].sector_map + trace->sector%LAYOUT_BLOCK;
	if(sector <=0 ) goto trace_error;
	disk_id = layouts[layout_id].disk_map;
	trace->nbytes = 512*2*8;

	if(0 > posix_memalign((void **)&buf, pagesize, trace->nbytes)){
			printf("posix_memalign error\n");
			sem_post(&submit_finished);
			exit(1);
	}

	memset(io, 0, sizeof(struct iocb));
	if(trace->rw == TRACE_READ){
		io_prep_pread(io, disks[disk_id].dev_id, buf,trace->nbytes, sector* 512);
	}
	else if(trace->rw == TRACE_WRITE){
		io_prep_pwrite(io, disks[disk_id].dev_id, buf,trace->nbytes, sector*512);	
	}
	trace->disk_id = disk_id;
	trace->layout_id = layout_id;
	io->data = trace;
	stall(gettime()-genesis_timeStamp,  trace->time_stamp-genesisTrace_timeStamp);
	if(io_submit(io_ctx, 1, &io) < 0){
		printf("submit error, sector %lld, disk %s\n", sector, disks[disk_id].dev_name);
		sem_post(&submit_finished);
		return arg;
	}
	if(maxios){
		if(maxios-- == 1){
			printf("submit finish\n");
			sem_post(&submit_finished);
			return arg;
		}
	}
	goto next_io;
}

//====================================================================================================================
//receive
//====================================================================================================================
static void *receive_th(void *arg)
{
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 100;
	int num = 0, i;
	struct io_event events[MAX_QUERY_LEN];
	while(1){
		num = io_getevents(io_ctx, 1, MAX_QUERY_LEN, events, NULL);
		for(i=0;i<num;i++){
			struct iocb *io = events[i].obj;
			iotrace * trace = io->data;
			if (events[i].res2 !=0 ) {//¶ÁÐ´Ê§°Ü
				printf("aio error\n");
				disks[trace->disk_id].aio_error++;
			}
			if(events[i].res != io->u.c.nbytes) {		
				disks[trace->disk_id].eios++;
				printf("io error %s %lld %lld %lu %lu %ld %d\n",disks[trace->disk_id].dev_name,trace->sector,trace->subsector,events[i].res , io->u.c.nbytes,trace->nbytes,trace->rw);
			}else{
				disks[trace->disk_id].num_access++;
			}
			layouts[trace->layout_id].num_access++;
			free(io->u.c.buf);
#ifndef LOCK_FREE_REPLAY
			add_to_list(iocb_list, io);
#else
			iocb_list[free_tail] = io;
			if(free_tail == MAX_QUERY_LEN) free_tail = 0;
			else free_tail++;
#endif
		}
	}
	return arg;
}
//====================================================================================================================
//replayer_init
//====================================================================================================================
int create_replayer(char *tracefile, char *tracetype, char *devs[], int ndev)
{
	int i;

	for(i = 0; i < ndev; i++){		
		disks[i].status = 1;
		disks[i].dev_id= open(devs[i], O_RDWR | O_DIRECT | O_NOATIME);
		if (disks[i].dev_id< 0){
			printf("open %s error\n", devs[i]);
			return -1;
		}
		strcpy(disks[i].dev_name, devs[i]);
	}
	ndisk = i;
	
	for(i=0; i < LAYOUT_MAX; i++){
		layouts[i].disk_map=i%ndisk;
		layouts[i].sector_map=i*LAYOUT_BLOCK;
		layouts[i].num_access=0;
	}
	for(i = sizeof(trace_engines)/sizeof(trace_engine_t) -1; i >= 0; i--){
		if(!strcmp(tracetype, (trace_engines+i)->type)) break;
	}
	if(i<0) {
		printf("no trace type support\n");
		return -1;
	}
	trace_type = i;
	trace_file = open_trace(tracefile);
	if(trace_file < 0) {
		printf("open trace file error\n");
		return -1;
	}
	if (pthread_create(&submit_tid, NULL, submit_th, NULL) != 0) {
		printf("create submit thread error\n");
		return -1;
	};
	
	if (pthread_create(&receive_tid, NULL, receive_th, NULL) != 0) {
		printf("create receive thread error\n");
		return -1;
	};
	return 0;
}

void wait_replay_done(void)
{
	int i;
	struct iocb * io;
	iotrace* trace;
	
	sem_wait(&submit_finished);return;
	for(i=0;i<MAX_QUERY_LEN;i++){
#ifndef LOCK_FREE_REPLAY
		io = (struct iocb*)get_from_list(iocb_list);
		trace = io->data;
#else
		while(free_head == free_tail) ;
		io = iocb_list[free_head];
		trace = io->data;
		if(free_head == MAX_QUERY_LEN - 1) free_head = 0;
		else free_head++;
#endif
		free(trace);
		free(io);
	}
	exit(0);
}
static void __attribute__((constructor)) replayer_init(void)
{
	int i;
#ifndef LOCK_FREE_REPLAY
	iocb_list = creat_list(MAX_QUERY_LEN);
	for(i=0; i< MAX_QUERY_LEN; i++){
		ios[i].data = trace +i;
		add_to_list(iocb_list, ios + i);
	}
#else
	for(i=0; i< MAX_QUERY_LEN; i++){
		iocb_list[i] = ios + i;
		ios[i].data = trace +i;
	}
	free_head = 0;
	free_tail = MAX_QUERY_LEN;
#endif
	sem_init(&submit_finished, 0, 0);

	memset(&io_ctx,0,sizeof(io_ctx));
	io_setup(MAX_QUERY_LEN,&io_ctx);

	printf("replayer int end\n");
}
