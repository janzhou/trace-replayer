#include <memory.h>
#include <stdio.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "DeviceManager.h"
#include "DeviceDriver.h"

static Connection *head_connection = NULL;

struct Drive drives[10];
static int drive_index = 0;

static Connection * get_device(int device_id){
	Connection * current_connection = head_connection;
	
	if(device_id>0) device_id--;
	else return NULL;
	
	while(current_connection && --device_id > 0)
		current_connection = current_connection->next;
	
	return current_connection;
}

int DriveReg(struct Drive * dri)
{
	int i;
	for(i = 0; i < drive_index; i++){
		if(!strcmp(drives[drive_index].type, dri->type)) break;
	}
	if(i != drive_index) return -1;
	memcpy(&drives[drive_index],dri,sizeof(struct Drive));
/*
	drives[drive_index].DeletePort = dri->DeletePort;
	drives[drive_index].DevConnect = dri->DevConnect;
	drives[drive_index].DeviceStart = dri->DeviceStart;
	drives[drive_index].DeviceStop = dri->DeviceStop;
	drives[drive_index].Disconnect = dri->Disconnect;
	drives[drive_index].GetPortInfo = dri->GetPortInfo;
	drives[drive_index].SetPort = dri->SetPort;
	strcpy(drives[drive_index].type, dri->type);
*/
	return drive_index++;
}

int DeviceConnect(struct Device * dev)
{
	int id;
	Connection *current_connection = NULL, * pconnection = head_connection;
	
	for(id = 0; id < drive_index; id++){
		if(!strcmp(drives[id].type, dev->type)) break;
	}

	if(id == drive_index) return -1;
	current_connection = malloc(sizeof(Connection));
	
	memcpy(&(current_connection->dev), dev, sizeof(struct Device));
	current_connection->drive_id = id;

	current_connection->sock_id = 0;
	current_connection->data = NULL;
	current_connection->next = NULL;
	if(0 > (current_connection->sock_id = (* drives[id].DevConnect)(current_connection))){
		free(current_connection);
		return -1;
	}
	//0 stdin 1 stdout 3 stderr
	if(3 > current_connection->sock_id && \
		 3 > (current_connection->sock_id = (* drives[id].DevConnect)(current_connection))){
		free(current_connection);
		return -1;
	}

	if(head_connection == NULL) head_connection = current_connection;
	else{
		while(pconnection->next) pconnection = pconnection->next;
		pconnection->next = current_connection;
	}
	return current_connection->sock_id;
}


int DeviceDisonnect(int device_id)
{
	if(device_id < 0) return -1;
	Connection * current_connection  = get_device(device_id);
	(* drives[current_connection->drive_id].DevDisconnect)(current_connection);
	return 0;
}



//返回设备信息
int GetDevices(Device devices[], int n)
{
	Connection * current_connection;
	int i = 0;
	
	if(n < 0) return -1;
	
	current_connection=head_connection;
	while(current_connection != NULL&&i < n){
		memcpy(&devices[i], &current_connection->dev,sizeof(struct Device));
		i++;
		current_connection=current_connection->next;
	}
	return i;
}

int GetPortsById(int device_id,PortInfo ports[],int n)
{
	Connection *current_connection = NULL;
	if(device_id < 0) return -1;
	if(n > 0){
		current_connection = head_connection;
		while(current_connection != NULL && --device_id >=0 )
			current_connection = current_connection->next;
		if(current_connection == NULL)
			return -1;
		return (* drives[current_connection->drive_id].GetPortInfo)(current_connection,ports, n);
	}
	else 
		return -1;
}

int StartSample(void)
{
	Connection * current_connection = head_connection;
	
	if(current_connection == NULL) return -1;
	
	while(current_connection != NULL){
		(* drives[current_connection->drive_id].DevStart)(current_connection);
		current_connection = current_connection->next;
	}
	return 0;
}

int StopSample(void)
{
	Connection * current_connection = head_connection;
	
	if(current_connection == NULL) return -1;
	
	while(current_connection != NULL){
		(* drives[current_connection->drive_id].DevStop)(current_connection);
		current_connection = current_connection->next;
	}
	return 0;
}

int SetPort(int device_id , int port_id , float * pValue)
{
	Connection * current_connection = get_device(device_id);

	if(current_connection == NULL){
		printf("no such device id\n");
		return -1;
	}

	return (* drives[current_connection->drive_id].SetPort)(current_connection, --port_id, pValue);
}

int DeletePort(int device_id , int port_id)
{
	Connection * current_connection;
	if(device_id < 1||port_id < 1)	
		return -1;

	current_connection = head_connection;
	while(current_connection && --device_id >= 0)
		current_connection = current_connection->next;
	if(current_connection == NULL)
		return -1;

	return (* drives[current_connection->drive_id].DeletePort)(current_connection,port_id);
}

int IsPortUsed(int device_id , int port_id)
{
	Connection * current_connection;
	if(device_id < 1||port_id < 1)
		return -1;

	current_connection = head_connection;
	while(current_connection && --device_id >= 0){
		current_connection = current_connection->next;
	}
	
	if(current_connection == NULL)
		return -1;
	return (* drives[current_connection->drive_id].IsPortUsed)(current_connection,port_id);
}
