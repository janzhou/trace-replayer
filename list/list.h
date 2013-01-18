#ifndef _H_LIST
#define _H_LIST

//#define _LOCK_FREE_LIST

#ifndef _LOCK_FREE_LIST
#include <semaphore.h>

typedef struct list_head{
	void * next;
	void * pre;
	sem_t sem;
	sem_t list_len;
	sem_t left_len;
}list_head;

list_head * creat_list(int max_length);
void add_to_list(list_head * head, void* node);
void* get_from_list(list_head * head);
void del_list(list_head * head);
int get_length_list(list_head * head, int* plen);
int get_leftlen_list(list_head * head, int* plen);
void* reget_from_list(list_head * head,  void* data);

#else //_LOCK_FREE_LIST
typedef struct list_head{
	int head, tail, max_len;
	void * node[1];
}list_head;

list_head * creat_list(int max_length);
void add_to_list(list_head * head, void* node);
void* get_from_list(list_head * head);
#endif
#endif
