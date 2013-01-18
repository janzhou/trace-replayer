#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


#include "replayer.h"
#include "feedback.h"

#include "devicemanager/DeviceManager.h"
#include "devicemanager/zyhd.h"

pthread_t model_predict;

float temperature[NUM_DISK] = {0.0}, power[NUM_DISK] = {0.0};
static void *model_predict_th(void *arg)
{
	float t[NUM_DISK][3] = {{0.0}}, dt[NUM_DISK][2] = {{0.0}};
	float tk1=0.8;
	int i, loop=0;
	sleep(1);
//	if(!(feedback&TEMPERATURE_FEEDBACK_ON)) return arg;

	Device dev;

	strcpy(dev.ip,"192.168.0.141");
	dev.port = 30282;
	strcpy(dev.type,"zyhd");
	ZYHD_init();
	
	if(DeviceConnect(&dev)<0) {printf("connect failed\n");exit(-1);}
	StartSample();
	SetPort(1, 9, &temperature[0]);
	SetPort(1, 12, &temperature[1]);
	SetPort(1, 10, &temperature[2]);

	for(i = 0; i < ndisk; i++) t[i][0] = 0;
	for(loop=0;loop < interval;loop++){//第一个600
		sleep(1);
		for(i = 0; i < ndisk; i++){
			t[i][0] += temperature[i];
			disks[i].t0 = disks[i].temperature = temperature[i];
		}
	}
	for(i = 0; i < ndisk; i++){
		t[i][2] = disks[i].t0 = disks[i].temperature = t[i][0]/interval;
	}
	while(!(feedback&TEMPERATURE_PREDICT_ON)){
		for(i = 0; i < ndisk; i++) t[i][0] = 0;
		for(loop=0;loop < interval;loop++){//第二个600
			sleep(1);
			for(i = 0; i < ndisk; i++){
				t[i][0] += temperature[i];
			}
		}
		for(i = 0; i < ndisk; i++){
			t[i][1] = disks[i].t0 = disks[i].temperature = t[i][0]/interval;
		}
	}

predict:
	for(i = 0; i < ndisk; i++) t[i][0] = 0;
	for(loop=0;loop < interval;loop++){//第二个600
		sleep(1);
		for(i = 0; i < ndisk; i++){
			t[i][0] += temperature[i];
		}
	}
	for(i = 0; i < ndisk; i++){
		t[i][0] = t[i][0]/interval;
		dt[i][0] = t[i][0] - t[i][1];
		dt[i][1] = t[i][1] - t[i][2];
		t[i][2] = t[i][1];
		t[i][1] = t[i][0];
		//对温度突变，快速响应
		if(t[i][0]>disks[i].temperature+(0.3*interval)/600 || t[i][0]+(0.3*interval)/600<disks[i].temperature){
			float k2 = t[i][0]>disks[i].temperature?(t[i][0]-disks[i].temperature):(disks[i].temperature-t[i][0]);
			disks[i].temperature = t[i][0] +dt[i][0]*tk1*k2;
			float j = t[i][0], k = dt[i][0];
			while(k > 0.0001 || k < -0.0001){
				k = k*tk1;
				j = j + k;
			}
			disks[i].t0 = j;//预测一次终点
			continue;
		}
		//平稳变化过程
		if(dt[i][1] != 0){
			if(dt[i][0]/dt[i][1] < 1 && dt[i][0]/dt[i][1] > 0.6){
				disks[i].temperature = t[i][0] + dt[i][0]*tk1;
			}else{
				disks[i].temperature = t[i][0];
			}
		}else{
			disks[i].temperature = t[i][0];
		}
		//终点预测及误差校准
		if((dt[i][0] < 0 && t[i][0]+(0.3*interval)/600< disks[i].t0) || (dt[i][0] > 0 && t[i][0] > disks[i].t0 + (0.3*interval)/600)){//(S[i-2] - T[i-2] > 0.3 || S[i-2] - T[i-2] < -0.3)
			float j = t[i][0], k = dt[i][0];
			while(k > 0.0001 || k < -0.0001){
				k = k*tk1;
				j = j + k;
			}
			disks[i].t0 = j;
		}

	}
	goto predict;
}

static void __attribute__((constructor)) model_predict_init(void)
{

	if (pthread_create(&model_predict, NULL, model_predict_th, NULL) != 0) {
		printf("create feedback thread error\n");
		exit(0);
	};

	printf("model_predict_th init end\n");
}

