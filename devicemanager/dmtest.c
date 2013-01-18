#include "DeviceManager.h"
#include "lmsens.h"
#include "zyhd.h"
#include <string.h>
#include <stdio.h>

static void callback(int sock_id, int event, void * buf, int len, void * callbackdata){}
int main()
{
	Device dev;float a;
	FILE* FP= fopen("dmtest","r");
	sock_server_bind(8000, 10, callback,NULL);
	strcpy(dev.ip,"192.168.0.141");
	dev.port = 30282;
	strcpy(dev.type,"zyhd");
	lmsens_init();
	ZYHD_init();
	
	if(DeviceConnect(&dev)<0) printf("connect failed\n");
	else  printf("connect success\n");
sleep(1);
	StartSample();
//	for(i=1;i<10;i++)
	SetPort(1, 20, &a);
	while(1){
		sleep(3);
	printf("a = %f\n",a);
		}
	return 0;
}
