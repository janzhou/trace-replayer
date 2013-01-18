#include <sys/socket.h>
#include <arpa/inet.h>
#include <malloc.h>
#include "shm.h"
#include "sock.h"
#include <string.h> 
#include <pthread.h>
#include <unistd.h>/*close*/

//======================================================================================
//======================receive and send=====================================================
//======================================================================================
int receive_socket(int sock_id,void *buf, int len)
{
	return recv(sock_id, buf, len, MSG_DONTWAIT);
}

int receive_socket_wait(int sock_id,void *buf, int len)
{
	return  recv(sock_id, buf, len, 0);
}

int send_socket(int sock_id,void *buf ,int len)
{
	return send(sock_id, buf, len, 0);
}


//======================================================================================
//======================client============================================================
//======================================================================================
typedef struct sock_list{
	int sock_id;
	void (* callback)(int,int,void *,int,void*);
	void * callbackdata;
	struct sock_list * next;
	pthread_t tid;
}sock_list;

sock_list * sock_head = NULL;

static void * receive_th(void * arg)
{
	sock_list * p_sock = arg;
	char receiveBuf[SOCK_BUF_LEN];
	int len = 0;

	if(p_sock->callback == NULL) return NULL;
	
	(* p_sock->callback)(p_sock->sock_id,SOCK_CONNECT,receiveBuf, len, p_sock->callbackdata);
	
	while(p_sock->sock_id){
		if( (len = receive_socket_wait(p_sock->sock_id, receiveBuf, SOCK_BUF_LEN)) <= 0 ){
			(* p_sock->callback) (p_sock->sock_id,SOCK_DISCONNECT,receiveBuf, len, p_sock->callbackdata);
			break;
		}
		else if(len > 0) (* p_sock->callback)(p_sock->sock_id,SOCK_DATAREADY,receiveBuf, len, p_sock->callbackdata);
	}

	return NULL;
}

int connect_socket(char * addr, int port, void (* callback)(int,int,void *,int,void*),void * callbackdata){
	struct sockaddr_in sin;
	sock_list * new_sock = (sock_list *)malloc(sizeof(sock_list));

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	//创建socket初始化
	if(inet_pton(AF_INET, addr, &sin.sin_addr) != 1){
		return 0;
	}

	if((new_sock->sock_id= socket(AF_INET, SOCK_STREAM, 0)) ==-1){
		return 0;
	}
	
	//连接服务器
	if(!connect(new_sock->sock_id, (struct sockaddr *) &sin, sizeof(sin))){
		new_sock->callback = callback;
		new_sock->callbackdata = callbackdata;
		if(callback && pthread_create(&new_sock->tid, NULL, receive_th, new_sock)){
			close(new_sock->sock_id);
			goto connect_error;
		}
		new_sock->next = sock_head;
		sock_head = new_sock;
		return new_sock->sock_id;
	}
connect_error:
	free(new_sock);
	return 0;
}

int disconnect_socket(int sock_id){	

	sock_list * temp;
	
	if(close(sock_id) == -1){
		return (-1);
	}
	
	if(sock_head->next==NULL){
		free(sock_head);
		sock_head=NULL;
	}
	else {
		for(temp=sock_head;temp->next!=NULL;temp=temp->next){
			if(temp->next->sock_id==sock_id){
				sock_list * tmp=temp->next;
				temp->next=temp->next->next;
				free(tmp);
				}
			}
		}
	return 0;
}

//======================================================================================
//======================server==========================================================
//======================================================================================
struct sv_callback{
	void (*callback)(int sock_id, int event, void * buf, int len, void * callbackdata);
	void *callbackdata;
};

struct accept_arg{
	int socketfd;
	struct sv_callback cb;
	pthread_t thread;
};

struct server_arg{
	int clientfd;
	struct sv_callback cb;
	pthread_t thread;
};

static void * server_th(void * arg)
{
	struct server_arg * s_arg = arg;
	if(s_arg->cb.callback){
		char receiveBuf[SOCK_BUF_LEN];
		int len;
		(*s_arg->cb.callback)(s_arg->clientfd, SOCK_CONNECT, NULL, 0,s_arg->cb.callbackdata);
		while( (len = receive_socket_wait(s_arg->clientfd, receiveBuf, SOCK_BUF_LEN))){
			(*s_arg->cb.callback)(s_arg->clientfd, SOCK_DATAREADY, receiveBuf, len,s_arg->cb.callbackdata);
		}
		(*s_arg->cb.callback)(s_arg->clientfd, SOCK_DISCONNECT, NULL, 0,s_arg->cb.callbackdata);
	}
	close(s_arg->clientfd);

	return NULL;
}

static void * server_accept_th(void * arg)
{
	struct accept_arg * ac_arg = arg;
	struct sockaddr_in clientAdd;
	socklen_t  len = sizeof(struct sockaddr_in);
	int clientfd;
	while((clientfd = accept(ac_arg->socketfd,(struct sockaddr *)&clientAdd,&len)) > 0){
		struct server_arg * s_arg = (struct server_arg *)malloc(sizeof(struct server_arg));
		s_arg->clientfd = clientfd;
		s_arg->cb= ac_arg->cb;
		if(pthread_create(&s_arg->thread,NULL,server_th,s_arg)){
			free(s_arg);
		}
	}
	return NULL;
}

int sock_server_bind(int port, int num, void (*callback)(int sock_id, int event, void * buf, int len, void * callbackdata),void *callbackdata)
{
	struct accept_arg * ac_arg = (struct accept_arg*)malloc(sizeof(struct accept_arg));

	ac_arg->cb.callback = callback;
	ac_arg->cb.callbackdata= callbackdata;
	
	ac_arg->socketfd = socket(AF_INET,SOCK_STREAM,0);
	if(ac_arg->socketfd == -1){
		goto bind_error;
	}

	struct sockaddr_in sa ;
	memset(&sa,0,sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(sa.sin_zero),8);

	if(bind(ac_arg->socketfd,(struct sockaddr *)&sa,sizeof(sa))!=0){
		goto bind_error;
	}

	if(listen(ac_arg->socketfd,num )!=0){
		goto bind_error;
	}
	
	if(pthread_create(&ac_arg->thread,NULL,server_accept_th,ac_arg)){
		goto bind_error;
	}

	return 0;
bind_error:
	free(ac_arg);
	return -1;
}
