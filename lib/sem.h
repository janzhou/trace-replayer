#ifndef SEM_SHM_H
#define SEM_SHM_H

#ifndef OPERATE_DIR
#define OPERATE_DIR "/tmp"
#endif

int create_sem(int key);
//删除信号量
void del_sem(int semid);
//v v操作
int v(int semid);

//p p操作
int p(int semid);

int create_sem_t(int key);

int create_key();

#endif
