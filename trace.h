#ifndef _TRACE_H
#define _TRACE_H

#define BT_MAX_PKTS 512
#define GENTRACE 0
#define BLKTRACE 1

#include <asm/types.h>

/*
 * Header for each bunch
 *
 * @nkts: 	Number of IO packets to process
 * @time_stamp:	Time stamp for this bunch of IOs
 */
struct io_bunch_hdr {
	__u64 npkts;
	__u64 time_stamp;
};

/*
 * IO specifer
 *
 * @sector:	Sector number of IO
 * @nbytes:	Number of bytes to process
 * @rw:		IO direction: 0 = write, 1 = read
 */
struct io_pkt {
	__u64 sector;
	__u64 nbytes;
	__u32 rw;
};

/*
 * Shorthand notion of a bunch of IOs
 *
 * @hdr: 	Header describing stall and how many IO packets follow
 * @pkts: 	Individual IOs are described here
 */
struct io_bunch {
	struct io_bunch_hdr hdr;
	struct io_pkt pkts[BT_MAX_PKTS];
};

/*
 * Header for each recorded file
 *
 * @version:	Version information
 * @genesis:	Time stamp for earliest bunch
 * @nbunches:	Number of bunches put into the file
 * @total_pkts:	Number of packets to be processed
 */
struct io_file_hdr {
	__u64 version;
	__u64 genesis;
	__u64 nbunches;
	__u64 total_pkts;
};

#define TRACE_WRITE 0
#define TRACE_READ 1
#define TRACE_NULL 2


struct trace_mode {
    __u32 rw;//0:rd, 1:wr, 2: rnd
    __u64 sectors;// disk size
    __u64 min_sec;// min io size
    __u64 ext_sec;// io size range
};


//外部接口
typedef struct iotrace{
	__u64 time_stamp;
	__u64 sector;
	long nbytes;
	int rw;// 0:read, 1:write, 2:null
	__u64 subsector;
	int disk_id;
	__u64 layout_id;
}iotrace;

typedef struct trace_engine_t{
	char type[64];
	int (*open_trace)(char *filename);
	int (*get_trace)(int trace_file, iotrace *);
	int (*close_trace)(int trace_file);
}trace_engine_t;

extern struct trace_engine_t trace_engines[3];


#endif //_TRACE_H
