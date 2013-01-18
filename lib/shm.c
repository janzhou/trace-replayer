#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <malloc.h>
#include"sem.h"
#include"shm.h"
#include <string.h>/*bzero*/

shm * create_shm_t(int key,int len){

	shm * pshm;
	
	pshm = (shm *)malloc(sizeof(shm));
	
	pshm->key=key;
	pshm->len = len;

	if(pshm->key==-1)
	{
		goto shm_error;
	}
	
	if((pshm->shmid=shmget(pshm->key,len,IPC_CREAT |0666)) == -1)
	{
		goto shm_error;
	}
										  
	pshm->addr=(void * )shmat(pshm->shmid,NULL,0);
	(pshm->semid)=create_sem_t(pshm->key);

	return pshm;

shm_error:
	free(pshm);
	return NULL;

}
shm * create_shm(int len){

	shm * pshm;
	
	pshm = (shm *)malloc(sizeof(shm));
	
	pshm->key= create_key();
	pshm->len = len;

	if(pshm->key==-1)
	{
		printf("key error\n");
		goto shm_error;
	}
	
	if((pshm->shmid=shmget(pshm->key,len,IPC_CREAT |0666)) == -1)
	{
		printf("shmget error %d\n", pshm->key);
		goto shm_error;
	}
										  
	pshm->addr=(void * )shmat(pshm->shmid,NULL,0);
	(pshm->semid)=create_sem(pshm->key);
	bzero(pshm->addr,len);

	return pshm;

shm_error:
	free(pshm);
	return NULL;

}

void del_shm(shm * pshm)
{
	shmdt(pshm->addr); 
	shmctl(pshm->shmid,IPC_RMID,NULL);
	del_sem(pshm->semid);
	free(pshm);
}
