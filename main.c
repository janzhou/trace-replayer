#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "replayer.h"
#include "feedback.h"

int main(int argc, char* argv[])
{
	char logfile[72];
	if(argc < 7){
		printf("usage: %s trace_file trace_type disks... maxios feedback interval logfile\n", argv[0]);
		return 0;
	}
	sprintf(logfile,"./log/%s",argv[argc-1]);
	thelog = fopen(logfile,"w");
	interval = atoi(argv[argc-2]);
	feedback = atoi(argv[argc-3]);
	maxios = atoi(argv[argc-4]);
	if(create_replayer(argv[1], argv[2], argv+3, argc - 3 - 4)==-1){//需要在1秒内启动，否则feedback会出错
		printf("create replayer error\n");
		return -1;
	}
	
	wait_replay_done();
	printf("finish replay\n");
	return 0;
}

