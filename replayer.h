#include <asm/types.h>

#ifndef REPLAYER_H
#define REPLAYER_H

typedef struct disk_t{
	int dev_id;
	char dev_name[10];
	float temperature;
	float t0;
	float red;
	float yellow;
	int status;
	__u64 num_access;
	__u64 aio_error;
	__u64 eios;
}disk_t;

typedef struct layout_t{
	int disk_map;
	__u64 sector_map;
	__u64 num_access;
}layout_t;

#define LAYOUT_MAX 65535
#define LAYOUT_BLOCK 2048000
#define NUM_DISK 20
#define MAX_QUERY_LEN 512

layout_t layouts[LAYOUT_MAX];
disk_t disks[NUM_DISK];
int ndisk;
int maxios;

int create_replayer(char *tracefile, char *tracetype, char *devs[], int ndev);
void wait_replay_done(void);

#endif



