#include<sys/types.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<netinet/in.h>
#include<malloc.h>
#include<pthread.h>

#include "lib/data.h"
#include "lm_sensors.h"
#include "sock.h"

#define PORT 8888
#define MAX_CLIENT_NUM 10
#define STDOUT_FILENO 0


int thread_funtion(int clientfd, int event,void *buf, int len ,void *callbackdata){

	switch(event){
		case  SOCK_CONNECT:
			socket_data_count = -1;
			(*do_the_real_work1)();
			{
				printf("get ports\n");
				int size = sizeof(struct lm_pkt_ports_return)+sizeof(sensors_socket_data)*socket_data_count;
				struct lm_pkt_ports_return * ports_return = malloc(size);
				ports_return->pkt_head.pkt_type = LM_PORTS_RETURN;
				ports_return->count = socket_data_count+1;
				for(i=0;i<=socket_data_count;i++)
					ports_return->ports[i] = socket_data[i];
				send_socket(clientfd,ports_return,size);
				free(ports_return);
			}

			break;

		case SOCK_DATAREADY:
			socket_data_count = -1;
			 (*do_the_real_work1)();
			{
				printf("get data\n");
				int size = sizeof(struct lm_pkt_data_return)+sizeof(float)*socket_data_count;
				struct lm_pkt_data_return * data_return = malloc(size);
				data_return->pkt_head.pkt_type = LM_DATA_RETURN;
				for(i=0;i<=socket_data_count;i++)
					data_return->data[i] = socket_data[i].data;
				send_socket(clientfd,data_return,size);
				free(data_return);
			}

			break;

		case SOCK_DISCONNECT:
			printf("end success!\n");
			close(clientfd);
			return 1;
			
		default:
			printf("Error\n");
		//	return -1;
	}
}

int socket_proc(int (*do_the_real_work1)(void))
{

	sock_server_bind(PORT,MAX_CLIENT_NUM,thread_funtion, NULL);

}
















