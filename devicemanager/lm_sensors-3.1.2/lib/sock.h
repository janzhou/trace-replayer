#ifndef _H_SOCK_
#define _H_SOCK_

#ifndef SOCK_BUF_LEN
#define SOCK_BUF_LEN 1024
#endif

#define SOCK_CONNECT		1
#define SOCK_DATAREADY		2
#define SOCK_DISCONNECT	3

int connect_socket(char * addr, int port, void (* callback)(int sock_id,int event,void * buf, int len, void * callbackdata), void * callbackdata);
int receive_socket(int sock_id,void *buf, int len);
int receive_socket_wait(int sock_id,void *buf, int len);
int send_socket(int sock_id,void *buf ,int len);
int disconnect_socket(int sock_id);

int sock_server_bind(int port, int num, void (*callback)(int sock_id, int event, void * buf, int len, void * callbackdata),void *callbackdata);
#endif