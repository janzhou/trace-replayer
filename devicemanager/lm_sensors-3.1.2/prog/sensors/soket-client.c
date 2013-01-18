#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero
#include "lm_sensors.h"

int main(int argc,char * argv[])
{
	if(argc!=2){
		printf("%s:IPAddress\n",argv[0]);
		return -1;
	}

	//int sockfd,numbytes;
	//char buf[100];
	struct sockaddr_in client_addr;
	//int i=0;

	bzero(&client_addr,sizeof(client_addr)); //把一段内存区的内容全部设置为0
    	client_addr.sin_family = AF_INET;    //internet协议族
   	 client_addr.sin_addr.s_addr = htons(INADDR_ANY);//INADDR_ANY表示自动获取本机地址
    	client_addr.sin_port = htons(0);    //0表示让系统自动分配一个空闲端口
    	//创建用于internet的流协议(TCP)socket,用client_socket代表客户机socket
    	int client_socket = socket(AF_INET,SOCK_STREAM,0);
    	if( client_socket < 0)
    	{
        	printf("Create Socket Failed!\n");
        	exit(1);
   	 }
   	 //把客户机的socket和客户机的socket地址结构联系起来
   	 if( bind(client_socket,(struct sockaddr*)&client_addr,sizeof(client_addr)))
   	 {
       	 printf("Client Bind Port Failed!\n"); 
       	 exit(1);
   	 }

	 
	 struct sockaddr_in server_addr;
   	 bzero(&server_addr,sizeof(server_addr));
    	server_addr.sin_family = AF_INET;
    	if(inet_aton(argv[1],&server_addr.sin_addr) == 0) //服务器的IP地址来自程序的参数
   	 {
       	 printf("Server IP Address Error!\n");
       	 exit(1);
    	}
    	server_addr.sin_port = htons(8888);
   	 socklen_t server_addr_length = sizeof(server_addr);
   	 //向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接
    	if(connect(client_socket,(struct sockaddr*)&server_addr, server_addr_length) < 0)
    	{
       	 printf("Can Not Connect To %s!\n",argv[1]);
        	exit(1);
   	    }

	char buf[512];struct lm_pkt_head* head = buf;
	int numbytes,nports=0,i;
	
	while(1)
	{
		scanf("%s",buf);
		switch(buf[0]){
			case '$':
				close(client_socket);
				return 1;
			case 'd':
				{
					struct lm_pkt_get_data get_data;
					get_data.pkt_head.pkt_type = LM_GET_DATA;
					if( send(client_socket,&get_data,sizeof(get_data),0)==-1)
					{
						perror("send error");
						return -1;
					}
				}
				break;
			case 'p':
				{
					struct lm_pkt_get_ports get_ports;
					get_ports.pkt_head.pkt_type = LM_GET_PORTS;
					if( send(client_socket,&get_ports,sizeof(get_ports),0)==-1)
					{
						perror("send error");
						return -1;
					}
				}
				break;
			case  's':
				{
					struct lm_pkt_head stop;
					stop.pkt_type = LM_STOP;
					if( send(client_socket,&stop,sizeof(stop),0)==-1)
					{
						perror("send error");
						return -1;
					}
				}
		}

		
		memset(buf,0 ,sizeof(buf));
		if((numbytes = recv(client_socket,buf,512,0))==-1)
		{
			perror("recv eoor");
			return -1;
		}
		printf("pkt_type %d\n",head->pkt_type);
		switch(head->pkt_type){
			case LM_PORTS_RETURN:
				{
					printf("ports return\n");
					struct lm_pkt_ports_return *ports_return = buf;
					printf("nports:%d\n",nports = ports_return->count);
					for(i=0;i<nports;i++){
						printf("%s\t%+6.1f%s\n",ports_return->ports[i].label,ports_return->ports[i].data,ports_return->ports[i].sulable);
					}
				}
				break;
			case LM_DATA_RETURN:
				{
					printf("data return\n");
					struct lm_pkt_data_return *data_return = buf;
					printf("data:");
					for(i=0;i<nports;i++){
						printf("%f ",data_return->data[i]);
					}
					printf("\n");
				}
				break;
			case LM_STOP:
				{
					printf("end return\n");
					close(client_socket);
					
				}
		}
		buf[numbytes]='\0';	
		printf("NUM:%d\n",numbytes);
		printf("result:%s\n",buf);		
	}
	close(client_socket);
	return 0;

}


