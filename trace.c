/*
	create by rayth at 2010.06.01
*/

/*
*	get trace from file;
*	sucess return 1,fail return 0;
*/
#include <unistd.h>
#include <stdio.h>//perror
#include <stdlib.h>//exit
#include <time.h>//time
#include <fcntl.h>//open


#include "trace.h"

#define GET_TRACE 100
#define random(x) (rand()%(x))

static int next_bunch(int ifd, struct io_bunch *bunch)
{
	int count, result;
	// read bunch record	
	// read bunch head
	result = read(ifd, &bunch->hdr, sizeof(bunch->hdr));
	if (result != sizeof(bunch->hdr)) {
		if (result == 0)   //end of file
			return 0;
		perror("Short hdr.");
		exit(-1);
	}
	//assert(bunch->hdr.npkts <= BT_MAX_PKTS);
	
	//read io_pkts
	count = bunch->hdr.npkts * sizeof(struct io_pkt);
	if(count >= BT_MAX_PKTS) printf("pkt error %d\n", count);
	result = read(ifd, &bunch->pkts, count);
	if (result != count) {
		perror("Short pkts.");
		exit(-1);
	}
	return 1;
}
struct io_bunch bunch;

int get_next_io_from_file(int fd,iotrace *trace)
{
    static int isHead=1;//printf("a;dsap dfkjdn\n");
    /*
     *====================================================
     *=======deal with the hdr=================================
     *====================================================
     */
    if(isHead == 1){
	struct io_file_hdr hdr;
	if (read(fd, &hdr, sizeof(hdr)) != sizeof(hdr)) {
	    perror( "Header read failed\n");
	    exit(-1);
	}
	isHead = 0;
    }
    /*
     *====================================================
     *=======deal with the hdr=================================
     *====================================================
     */

    static __u64 i = 0;

    if(i == bunch.hdr.npkts) {
	if(next_bunch(fd,&bunch) <=0){
	    //printf("endof the tracefile\n");
	    isHead=1;
	    lseek(fd,0,SEEK_SET);
		trace->rw = TRACE_NULL;
	    return 0;
	}
	else	{
	    i = 0;				
	}
    }

    trace->rw = bunch.pkts[i].rw;
    trace->nbytes = bunch.pkts[i].nbytes;
    trace->sector = bunch.pkts[i].sector;
    trace->time_stamp = bunch.hdr.time_stamp;
    i++;
    return 1;
}

static int
open_blktrace(char *filename)
{
    int fd;
    fd = open(filename, O_RDWR);
    if (fd < 0){
	exit(-1);
    }
    return fd;
}

static int
get_blktrace(int fd, iotrace *trace)
{
    return get_next_io_from_file(fd, trace);
}

static int close_blktrace(int fd)
{
    close(fd); 
    return 0;
}

int
close_tracefile(int fd)
{
    if (fd) {
	close_blktrace(fd); }
    return 0;
}

//spc
FILE * spcfp;
static int open_spc_trace(char * filename)
{
	spcfp = fopen(filename, "r");
	return 1;
}

static int get_spc_trace(int fd, iotrace* trace)
{
    char buf[512];
	int asu;
	char opcode;
	float time_stamp;

	if(fgets(buf, 512, spcfp) == NULL){
		trace->rw = TRACE_NULL;
		return 0;
	}
	sscanf(buf,"%d,%lld,%ld,%c,%f", &asu,&trace->sector,&trace->nbytes,&opcode,&time_stamp);
	trace->time_stamp = time_stamp*1000000000;
	if(opcode == 'r' || opcode == 'R'){
		trace->rw = TRACE_READ;
	} else trace->rw = TRACE_WRITE;
	
	return fd;
}

static int close_spc_trace(int fd)
{
	fclose(spcfp);
	return fd;
}

//iotta
FILE * blkparsefp;
static int open_blkparse_trace(char * filename)
{
//   int fd = 0;
//   fd = open(filename, O_RDWR);
   blkparsefp = fopen(filename, "r");
   return 1;
}

static int get_blkparse_trace(int fd, iotrace* trace)
{
	char buf[512];
	int pid,x,y;
	char p[36];
	char h[36];
	char op;

	if(fgets(buf, 512, blkparsefp) == NULL){
		trace->rw = TRACE_NULL;
		return 0;
	}else{
		//93596865840978 318 kjournald 15961104 8 W 2 0 4dcfd7e0f148bcc659c4baafc813c773
//		printf("%s", buf);
		sscanf(buf, "%lld %d %s %lld %ld %c %d %d %s", &trace->time_stamp, &pid, p, &trace->sector, &trace->nbytes, &op, &x, &y, h);
//		printf("%lld %ld %s %lld %lld %c %d %d %s\n", trace->time_stamp, pid, p, trace->sector, trace->nbytes, op, x, y, h);
		trace->nbytes *= 512;
		if(op == 'W') trace->rw = TRACE_WRITE;
		if(op == 'R') trace->rw = TRACE_READ;
	}
	return fd;
}

static int close_blkparse_trace(int fd)
{
	fclose(blkparsefp);
	return fd;
}

struct trace_engine_t trace_engines[] = {
		{"blktrace", open_blktrace, get_blktrace, close_blktrace},
		{"blkparse", open_blkparse_trace, get_blkparse_trace, close_blkparse_trace},
		{"spc", open_spc_trace, get_spc_trace, close_spc_trace},
};


