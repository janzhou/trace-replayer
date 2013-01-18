#ifndef LIB_SHM_H
#define LIB_SHM_H

#ifndef SHM_DIR
#define SHM_DIR "/tmp"
#endif

typedef struct shm{
	int shmid;
	int semid;
	int key;
	int len;
	void * addr;
}shm;
shm * create_shm_t(int key,int len);
shm * create_shm(int len);
void del_shm(shm * pshm);
#endif