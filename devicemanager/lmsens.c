#include "../lib/sock.h"
#include <stdio.h>
#include<string.h>

#include <errno.h>
#include <pthread.h>
#include"lmsens.h"
#include"DeviceDriver.h"

#include "lmsensNetProtoDef.h"

struct lm_sensors_info{
	char label[20];
	char sulabe[10];
	float*  pdata;
};

struct lm{
	int count;
	int do_sample;
	struct lm_sensors_info ports[1];
};


struct lm_sensors_info lm_info[64];

static int drive_id = -1;

static void LM_TCP_Callback(int sock_id,int event,void * buf, int len, void * callbackdata)
{
	Connection * current_connection = callbackdata;
	struct lm * lmsens = current_connection->data;
	struct lm_pkt_head* pkt_head = buf;
	switch(event){
		case SOCK_CONNECT:
			//连上之后应该召唤有多少端口，然后再分配内存
			{
				struct lm_pkt_get_ports *get_ports = buf;
				get_ports->pkt_head.pkt_type = LM_GET_PORTS;
				send_socket(sock_id, get_ports, sizeof(struct lm_pkt_get_ports));
			}
			break;
		case SOCK_DATAREADY:
			switch(pkt_head->pkt_type){
				case LM_DATA_RETURN:
					{
						struct lm_pkt_data_return *data_return = buf;
						int i;
						for(i = 0; i < lmsens->count; i++){
							if(lmsens->ports[i].pdata)
								*lmsens->ports[i].pdata = data_return->data[i];
						}
						if(lmsens->do_sample){
							struct lm_pkt_get_data * get_data = buf;
							sleep(1);
							get_data->pkt_head.pkt_type = LM_GET_DATA;
							send_socket(sock_id,buf,sizeof(struct lm_pkt_get_data));
						}
					}
					break;
				case LM_PORTS_RETURN:
					{
						struct lm_pkt_ports_return * ports_return = buf;int i;
						if(!current_connection->data){
							current_connection->data = malloc(sizeof(struct lm)+(ports_return->count-1)*sizeof(struct lm_sensors_info));
							lmsens = current_connection->data;
							lmsens->count = ports_return->count;
							for(i = 0; i < lmsens->count; i++){
								strcpy(lmsens->ports[i].label ,ports_return->ports[i].label);
								strcpy(lmsens->ports[i].sulabe,ports_return->ports[i].sulable);
								lmsens->ports[i].pdata = NULL;
							}
						}
					}
					break;
			}
			break;
		case SOCK_DISCONNECT:
			break;
	}
}

static int LMSENS_Connect(Connection * current_connection){                                                   
	if(current_connection->drive_id != drive_id) return -1;
	
	if((current_connection->sock_id = connect_socket(current_connection->dev.ip,current_connection->dev.port, LM_TCP_Callback, current_connection)) < 0){
			printf("TCP connect fail.");                                              
	}

	return current_connection->sock_id;
}           

static int LMSENS_DeviceStart(Connection * current_connection )
{
	struct lm_pkt_get_data get_data;
	struct lm * lmsens = current_connection->data;
	if(!lmsens) return -1;
	
	lmsens->do_sample = 1;
	get_data.pkt_head.pkt_type = LM_GET_DATA;
	send_socket(current_connection->sock_id, &get_data,sizeof(struct lm_pkt_get_data));
	return 0;
}

static int LMSENS_DeviceStop(Connection *current_connection){                                                    
	struct lm * lmsens = current_connection->data;
	lmsens->do_sample = 0;
	return 0;                                                                                     
	                                                                                              
}                                                                                                 

static int LMSENS_Disconnect(Connection *current_connection){                                                                 
	struct lm_pkt_head head;
	head.pkt_type = LM_STOP;                                                                                               

	send_socket(current_connection->sock_id, &head, sizeof(struct lm_pkt_head));
	if(disconnect_socket(current_connection->sock_id) == -1){
		printf("Fail to close\n");
		return (-1);
	}               
	return 0;                                                                                     
	                                                                                              
}  

static int LMSENS_GetPortInfo(Connection * current_connection, PortInfo PInfo[],int n){
	return 0;
}

static int LMSENS_SetPort(Connection * current_connection, int port_id, float * pValue){
	struct lm * lmsens = current_connection->data;
	if(!lmsens) return -1;
	printf("portid %d",port_id);
	if(port_id<1) return -1;
		lmsens->ports[port_id-1].pdata= pValue;
	return 0;
}

static int LMSENS_IsPortUsed(Connection * current_connection, int port_id){
	return 0;
}

static int LMSENS_DeletePort(Connection * current_connection, int port_id){
	return 0;
}

int lmsens_init(void)
{
	Drive dri;
	dri.DeletePort = LMSENS_DeletePort;
	dri.DevConnect = LMSENS_Connect;
	dri.DevDisconnect = LMSENS_Disconnect;
	dri.DevStart = LMSENS_DeviceStart;
	dri.DevStop = LMSENS_DeviceStop;
	dri.GetPortInfo = LMSENS_GetPortInfo;
	dri.IsPortUsed = LMSENS_IsPortUsed;
	dri.SetPort = LMSENS_SetPort;
	printf("brea  -- %d ",LMSENS_DeviceStart);
	strcpy(dri.type,"lm_sensors");

	return (drive_id= DriveReg(&dri));
}


