#ifndef SEM_SHM_H
#define SEM_SHM_H

#ifndef OPERATE_DIR
#define OPERATE_DIR "/tmp"
#endif

int create_sem(int key);
//ɾ���ź���
void del_sem(int semid);
//v v����
int v(int semid);

//p p����
int p(int semid);

int create_sem_t(int key);

int create_key();

#endif
