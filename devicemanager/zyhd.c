#include "zyhd.h"
#include "ZH2XNetProtoDef.h"
#include "../lib/sock.h"
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>//sleep

#define BUF_LENGTH 0x3E38
static int drive_id;

struct zyhd{
	int do_sample;
	int bufLen;
	char buf[BUF_LENGTH];
	float * port[32];
};

static inline void zyhd_calldata(int sock_id){
	PACKET_HEADER calldata;
	calldata.type = PTYPE_RTW_080826;
	calldata.length = 0;	
	send_socket(sock_id, &calldata, sizeof(calldata));
}


//供面板上的开始按钮的回调函数所调用 
/*发送开始报文*/
//中元华电，启动采样
static int  ZYHD_DeviceStart(Connection* current_connection)								  
{
	START_PACKET start;
	int i;

	start.header.type = PTYPE_CENTERDATA;
	start.header.length = sizeof(CENTER_DATA);
	
	for(i=0;i<64;i++) 
		start.data.szUser[i] = 0;
	start.data.nFlag=0;		
	start.data.nAddr=0;
	start.data.nChnl = 32; //指定通道数

	for(i = 0; i<32;i++)  start.data.Chnls[i] = i;
	for(; i<255;i++) start.data.Chnls[i] = 0;

	if(send_socket(current_connection->sock_id, &start, sizeof(START_PACKET)) < 0){
		printf("ZYHD %s device start send error\n",current_connection->dev.ip);
		return -1;
	}

	return 0;

}


//供上层面板上的结束按钮的回调函数所调用
/*发送停止报文*/
//中元华电停止采样
static int ZYHD_DeviceStop(Connection* current_connection)										
{
	
	struct zyhd *str_zyhd = current_connection->data;

	str_zyhd->do_sample = 1;
	
	return 0;

}

static void ZYHD_TCP_CALLBACK(int sock_id,int event,void * buf,int len,void * callbackdata)
{
	PACKET_HEADER * pkt_head = buf;
	Connection* current_connection = callbackdata;
	struct zyhd *str_zyhd = current_connection->data;
	switch(event){
		case SOCK_CONNECT:
			break;
		case SOCK_DATAREADY:
			//printf("zyhd data len %d\n",str_zyhd->bufLen);
			switch(str_zyhd->do_sample){
				case 0:
					if(pkt_head->type == PTYPE_RETURN && pkt_head->length == 0) str_zyhd->do_sample++;
					break;
				case 1:
					if(pkt_head->type == PTYPE_RETURN && pkt_head->length == 0) str_zyhd->do_sample++;
					zyhd_calldata(sock_id);
					break;
				case 2:
					pkt_head = (PACKET_HEADER *)(str_zyhd->buf);
					if(pkt_head->type != 0x01000100 || pkt_head->length != 0x25) str_zyhd->bufLen = 0;
					memcpy(str_zyhd->bufLen+str_zyhd->buf, buf, len);
					str_zyhd->bufLen += len;
					if(str_zyhd->bufLen >= 0x3cf0){
						int i;
						RTWDATAS_080826 * data = ( RTWDATAS_080826 * )(str_zyhd->buf + 0x25 + sizeof(PACKET_HEADER) + sizeof(PACKET_HEADER));
						str_zyhd->bufLen = 0;
						printf("zyhd data ready");
						for(i = 0; i < 8; i++){
							if(str_zyhd->port[i] != NULL){
								printf(" %d-%f", i+1,data->Chnls[i].fvir0);
								*(str_zyhd->port[i]) = data->Chnls[i].fvir0;
							}
						}
						//temperature
						for(i = 8; i < 15; i++){
							if(str_zyhd->port[i] != NULL){
								printf(" %d-%f", i+1,6250*(data->Chnls[i].fvir0) - 25.0);
								*(str_zyhd->port[i]) = 6250*(data->Chnls[i].fvir0) - 25.0;
							}
						}
						for(i = 15; i < 32; i++){
							if(str_zyhd->port[i] != NULL){
								printf(" %d-%f", i+1,data->Chnls[i].fvir0);
								*(str_zyhd->port[i]) = data->Chnls[i].fvir0;
							}
						}
						printf("\n");
						sleep(1);
						zyhd_calldata(sock_id);
					}
					break;
			}
			break;
		case SOCK_DISCONNECT:
			break;
	}
}

static int  ZYHD_Connect(Connection* current_connection)
{
	if(current_connection->drive_id != drive_id) return -1;
	printf("zyhd %s %d %s\n", current_connection->dev.ip, current_connection->dev.port, current_connection->dev.type);
	if((current_connection->sock_id = connect_socket(current_connection->dev.ip,current_connection->dev.port, ZYHD_TCP_CALLBACK,current_connection)) < 0){
		printf("zyhd tcp connect fail %s %d\n", current_connection->dev.ip,current_connection->dev.port);       
		return 0;                                       
	}

	current_connection->data = malloc(sizeof(struct zyhd));
	memset(current_connection->data, 0, sizeof(struct zyhd));
	
	return current_connection->sock_id;
}

static int  ZYHD_Disconnect(Connection * current_connection)
{
	if(disconnect_socket(current_connection->sock_id) == -1){
		printf("Fail to close\n");
		return (-1);
	}              
	free(current_connection->data);
	current_connection->data = NULL;
	return 0;
}

static int ZYHD_GetPortInfo(Connection * current_connection, PortInfo PInfo[],int n){
	return 0;
}

static int ZYHD_SetPort(Connection * current_connection, int port_id, float * pValue){
	struct zyhd *str_zyhd = current_connection->data;	
	str_zyhd->port[port_id] = pValue;
	return 0;
}

static int  ZYHD_IsPortUsed(Connection * current_connection, int port_id){
	return 0;
}

static int  ZYHD_DeletePort(Connection * current_connection, int port_id){
	return 0;
}

int ZYHD_init(void)
{
	Drive dri;
	dri.DeletePort = ZYHD_DeletePort;
	dri.DevConnect = ZYHD_Connect;
	dri.DevDisconnect = ZYHD_Disconnect;
	dri.DevStart = ZYHD_DeviceStart;
	dri.DevStop =  ZYHD_DeviceStop;
	dri.GetPortInfo = ZYHD_GetPortInfo;
	dri.IsPortUsed = ZYHD_IsPortUsed;
	dri.SetPort = ZYHD_SetPort;
	strcpy(dri.type,"zyhd");

	return (drive_id= DriveReg(&dri));
}
