#include <sys/sem.h> 
#include "sem.h"
#include <string.h>
#include <stdio.h>

union semun {
	 int val;
	 struct semid_ds *buf;
	 unsigned short *array;
};

//生成信号量
int create_sem_t(int key)
{
 	union semun sem;
 	int semid;
	sem.val = 0;
 	semid = semget(key,1,IPC_CREAT|0666);
 	if (-1 == semid){
 		 printf("create semaphore error\n");
 		 return(-1);
 	}
	return semid;
}

int create_key()
{
	static int ftok_id = 0;
	return ftok(OPERATE_DIR,ftok_id++);
}

int create_sem(int key)
{
 	union semun sem;
 	int semid;
	sem.val = 0;
 	semid = semget(key,1,IPC_CREAT|0666);
 	if (-1 == semid){
 		 printf("create semaphore error\n");
 		 return(-1);
 	}
 	semctl(semid,0,SETVAL,sem);
	return semid;
}

//删除信号量
void del_sem(int semid)
{
	 union semun sem;
 	sem.val = 0;
 	semctl(semid,0,IPC_RMID,NULL);
}

//v v操作
int v(int semid)
{
 	struct sembuf sops={0,+1,0};
 	return (semop(semid,&sops,1));
}

//p p操作
int p(int semid)
{
 	struct sembuf sops={0,-1,0};
 	return (semop(semid,&sops,1));
}
