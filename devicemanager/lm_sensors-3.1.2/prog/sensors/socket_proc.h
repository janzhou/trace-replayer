#include<sys/types.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include"lib/data.h"
#include<netinet/in.h>
#include<pthread.h>

int socket_proc();