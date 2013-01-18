#include "ks706.h"
#include "stdio.h"

//----------------------------------------------------------------------------------------------------------------------------------------
//KS706
//---------------------------------------------------------------------------------------------------------------------------------------- 

int KS706_DeviceStart(Connection * current_connection)								  
{

	if(current_connection->device.type != KS706) {
		printf("KS706: Device type does not match\n");
//		return -1;
	}
	
	char set[144];

	//read from file to get set[]
	//read()
	set[0] = 104;
	set[1] = 1;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = 49;
	set[6] = 49;
	set[7] = 22;
	if(send_socket(current_connection->handle, set, 8) < 0) return -1;
	sleep(100);
	
	//read();
	set[0] = 104;
	set[1] = 1;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = 92;
	set[6] = 92;
	set[7] = 22;
	if(send_socket(current_connection->handle, set, 8) < 0) return -1;
	sleep(1);
	if(send_socket(current_connection->handle, set, 8) < 0) return -1;
	sleep(1);
	if(send_socket(current_connection->handle, set, 8) < 0) return -1;
	sleep(1);			 
	
	//read();
	set[0] = 104;
	set[1] = 18;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = -1;
	set[6] = 0;
	set[7] = 0;
	set[8] = 7;
	set[9] = 0;
	set[10] = 0;
	set[11] = 0;
	set[12] = 0;
	set[13] = 0;
	set[14] = 0;
	set[15] = 0;
	set[16] = 0;
	set[17] = 0;
	set[18] = 0;
	set[19] = 0;
	set[20] = 0;
	set[21] = 0;
	set[22] = 0;
	set[23] = 6;
	set[24] = 22;
	if(send_socket(current_connection->handle, set, 8) < 0) return -1;
	sleep(1);
	if(send_socket(current_connection->handle, set, 8) < 0) return -1;
	sleep(1);
	if(send_socket(current_connection->handle, set, 8) < 0) return -1;
	sleep(1);
	
	//read(int FileHandle,void *Buffer,unsigend int NumberofBytes);
	set[0] = 104;
	set[1] = 18;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = -6;
	set[6] = 0;
	set[7] = 96;
	set[8] = -66;
	set[9] = 18;
	set[10] = 0;
	set[11] = 56;
	set[12] = -79;
	set[13] = 18;
	set[14] = 0;
	set[15] = 1;
	set[16] = 0;
	set[17] = 0;
	set[18] = 0;
	set[19] = 0;
	set[20] = 81;
	set[21] = 37;
	set[22] = 2;
	set[23] = -98;
	set[24] = 22;
	if(send_socket(current_connection->handle, set, 8) < 0) return -1;
	sleep(1);  

	pthread_create(NULL, NULL, KS706_DoSample, (void *)current_connection);
	printf("pthread\n");
	return 0;

} 



//供上层面板上的结束按钮的回调函数所调用
/*发送停止报文*/
int KS706_DeviceStop(Connection * current_connection)										
{
	char set[144];
												  
	//read(int FileHandle,void *Buffer,unsigned int NumberofBytes);
	set[0] = 104;
	set[1] = 1;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = 50;
	set[6] = 50;
	set[7] = 22;
	if(send_socket(current_connection->handle, set, 8) < 0) return -1;
	sleep(1);			 
	
	//ReadLine(int FileHandle,char LineBuffer[],int MaximumBytes);
	set[0] = 104;
	set[1] = 18;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = -7;
	set[6] = 0;
	set[7] = 0;
	set[8] = 0;
	set[9] = 0;
	set[10] = 0;
	set[11] = 0;
	set[12] = 0;
	set[13] = 0;
	set[14] = 0;
	set[15] = 1;
	set[16] = 0;
	set[17] = 0;
	set[18] = 0;
	set[19] = 0;
	set[20] = 0;
	set[21] = 0;
	set[22] = 0;
	set[23] = -7;
	set[24] = 22;
	if(send_socket(current_connection->handle, set, 8) < 0) return -1;
	sleep(1);

	*(int*) current_connection->data = 0;
	return 0;

} 

static void * KS706_DoSample (void * point)
{
	Connection * current_connection = (Connection *)point;
	int length = 0,bufPoint = 0;
	char buf[BUF_LENGTH];
	int i = 1;

	current_connection->data = &i;
	while(i){
		printf("KS706 call data\n");
		if((length = receive_socket(current_connection->handle, buf + bufPoint, BUF_LENGTH)) <0)	continue;	   //BUF_LENGTH
		
		if(length < 2174){
			bufPoint += length;
			if(bufPoint < 2174)	continue;
		}
		bufPoint = 0;
		printf("KS709 data ready\n");
		if(current_connection->port[0] != NULL)
			*(current_connection->port[0]) = *(float *)(buf + 6);
		if(current_connection->port[1] != NULL)
			*(current_connection->port[1]) = *(float *)(buf + 6);
		if(current_connection->port[2] != NULL)
			*(current_connection->port[2]) = *(float *)(buf + 6);
		if(current_connection->port[3] != NULL)
			*(current_connection->port[3]) = *(float *)(buf + 6);
		if(current_connection->port[4] != NULL)
			*(current_connection->port[4]) = *(float *)(buf + 6);
		if(current_connection->port[5] != NULL)
			*(current_connection->port[5]) = *(float *)(buf + 6);
		if(current_connection->port[6] != NULL)
			*(current_connection->port[6]) = *(float *)(buf + 6);
		if(current_connection->port[7] != NULL)
			*(current_connection->port[7]) = *(float *)(buf + 6);
		if(current_connection->port[8] != NULL)
			*(current_connection->port[8]) = *(float *)(buf + 6);
		if(current_connection->port[9] != NULL)
			*(current_connection->port[9]) = *(float *)(buf + 6);
		if(current_connection->port[10] != NULL)
			*(current_connection->port[10]) = *(float *)(buf + 6);
		if(current_connection->port[11] != NULL)
			*(current_connection->port[11]) = *(float *)(buf + 6);
		if(current_connection->port[12] != NULL)
			*(current_connection->port[12]) = *(float *)(buf + 6);
		if(current_connection->port[13] != NULL)
			*(current_connection->port[13]) = *(float *)(buf + 6);
		if(current_connection->port[14] != NULL)
			*(current_connection->port[14]) = *(float *)(buf + 6);
		if(current_connection->port[15] != NULL)
			*(current_connection->port[15]) = *(float *)(buf + 6);
		if(current_connection->port[16] != NULL)
			*(current_connection->port[16]) = *(float *)(buf + 6);
		if(current_connection->port[17] != NULL)
			*(current_connection->port[17]) = *(float *)(buf + 6);
		if(CBfunction != NULL) (* CBfunction)(DATAREADY,CBdata);
		 goto exit;
	}		
	
exit:
	return NULL;
	
}

Connection * KS706_Connect(Device_Node* device)
{
	char set[144];
	Connection * current_connection = NULL;
	int connect_id = 0;

	if(device->type != ZYHD) return NULL;

	if((connect_id = DeviceConnect(device)) == 0){
			return NULL;
	}

	set[0] = 104;
	set[1] = 1;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = 48;
	set[6] = 48;
	set[7] = 22;
	if(send_socket(connect_id, set, 8) < 0) {disconnect_socket(connect_id); return NULL;}
	sleep(1);
	//read();
	set[0] = 104;
	set[1] = 1;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = 94;
	set[6] = 94;
	set[7] = 22;
	if(send_socket(connect_id, set, 8) < 0) {disconnect_socket(connect_id); return NULL;}
	sleep(1);
	
	//read();
	set[0] = 104;
	set[1] = 1;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = 93;
	set[6] = 93;
	set[7] = 22;
	if(send_socket(connect_id, set, 8) < 0) {disconnect_socket(connect_id); return NULL;}
	sleep(1);
	
	//read();
	set[0] = 104;
	set[1] = 1;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = 88;
	set[6] = 88;
	set[7] = 22;
	if(send_socket(connect_id, set, 8) < 0) {disconnect_socket(connect_id); return NULL;}
	sleep(1);
	
	//read();
	set[0] = 104;
	set[1] = 1;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = 87;
	set[6] = 87;
	set[7] = 22;
	if(send_socket(connect_id, set, 8) < 0) {disconnect_socket(connect_id); return NULL;}
	sleep(1);
	
	//read();
	set[0] = 104;
	set[1] = 1;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = 110;
	set[6] = 110;
	set[7] = 22;
	if(send_socket(connect_id, set, 8) < 0) {disconnect_socket(connect_id); return NULL;}
	sleep(1);
	
	//read();
	set[0] = 104;
	set[1] = 1;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = 109;
	set[6] = 109;
	set[7] = 22;
	if(send_socket(connect_id, set, 8) < 0) {disconnect_socket(connect_id); return NULL;}
	sleep(1);
	
	//read();
	set[0] = 104;
	set[1] = 1;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = 104;
	set[6] = 104;
	set[7] = 22;
	if(send_socket(connect_id, set, 8) < 0) {disconnect_socket(connect_id); return NULL;}
	sleep(1);
	
	//read();
	set[0] = 104;
	set[1] = 1;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = 103;
	set[6] = 103;
	set[7] = 22;
	if(send_socket(connect_id, set, 8) < 0) {disconnect_socket(connect_id); return NULL;}
	sleep(1);


	current_connection = (Connection*)malloc(sizeof(Connection));

	current_connection->next = NULL;
	current_connection->handle = connect_id;
	current_connection->device = * device;

	return current_connection;
}




int  KS706_Disconnect(Connection * current_connection)
{
	char set[144]; 
	
	//维护链表
	
	//read();
	set[0] = 104;
	set[1] = 18;
	set[2] = 0;
	set[3] = 0;
	set[4] = 0;
	set[5] = -1;
	set[6] = 0;
	set[7] = 0;
	set[8] = -1;
	set[9] = 0;
	set[10] = 0;
	set[11] = 0;
	set[12] = 0;
	set[13] = 0;
	set[14] = 0;
	set[15] = 0;
	set[16] = 0;
	set[17] = 0;
	set[18] = 0;
	set[19] = 0;
	set[20] = 0;
	set[21] = 0;
	set[22] = 0;
	set[23] = -2;
	set[24] = 22;
	if(send_socket(current_connection->handle, set, 8, 0) < 0) return -1;
	sleep(1);
	if(disconnect_socket(current_connection->handle) == -1){
		perror("Fail to close");
		return (-1);
	}
	current_connection->handle = 0;
	return 0;
} 

