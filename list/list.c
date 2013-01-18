#include <malloc.h>
#include<pthread.h>

#include "list.h"

#ifndef _LOCK_FREE_LIST
typedef struct list_node{
	struct list_node * next;
	struct list_node * pre;
	void * data;
}list_node;

void add_to_list(list_head * head, void* data)
{//printf("add_to_list\n");
	list_node * node = NULL;
	if(!head) return;

	node = malloc(sizeof(list_node));
	node->data = data;
	node->next = (list_node *)head;

	sem_wait(&head->left_len);
	sem_wait(&head->sem);
	node->pre = head->pre;
	((list_node*)head->pre)->next = (void *)node;
	head->pre = node;
	sem_post(&head->sem);
	sem_post(&head->list_len);	
}

static inline void init_list(list_head * head, int max_length)
{
	head->next = head->pre = head;
	sem_init(&head->sem, 0, 1);
	sem_init(&head->list_len, 0, 0);
	sem_init(&head->left_len, 0, max_length);
}

list_head * creat_list(int max_length)
{
	list_head* head = malloc(sizeof(list_head));
	init_list(head, max_length);
	return head;
}

void del_list(list_head * head)
{
	if(!head) return;
	sem_destroy(&head->sem);
	free(head);
}

void* get_from_list(list_head * head)
{
	list_node * node = NULL;
	void* data = NULL;
	
	if(!head) return NULL;

	sem_wait(&head->list_len);
	sem_wait(&head->sem);
	if(head->next != head){
		node = head->next;
		node->next->pre = (void *)head;
		head->next = node->next;
	}
	sem_post(&head->sem);
	sem_post(&head->left_len);

	if(node){
		data = node->data;
		free(node);
	}
	return data;
}

void* reget_from_list(list_head * head,  void* data)
{
	list_node * node = NULL;
	void* redata = data;
	
	if(!head) return NULL;

	sem_wait(&head->sem);
	if(head->next != head){
		node = head->next;
		//detach from head
		node->next->pre = (void *)head;
		head->next = node->next;
		
		redata = node->data;
		node->data = data;
		//add to tail
		node->next = (list_node *)head;
		node->pre = head->pre;
		((list_node*)head->pre)->next = (void *)node;
		head->pre = node;
	}
	sem_post(&head->sem);

	return redata;
}

int get_length_list(list_head * head, int* plen)
{
	return sem_getvalue(&head->list_len, plen);
}

int get_leftlen_list(list_head * head, int* plen)
{
	return sem_getvalue(&head->left_len, plen);
}

void exchange_node(list_head *shead, list_head *dhead){
	void * node;
	sem_wait(&shead->list_len);
	sem_wait(&shead->sem);
	node = get_from_list(shead);
	sem_post(&shead->list_len);
	sem_post(&shead->sem);

	sem_wait(&dhead->left_len);
	sem_wait(&dhead->sem);
	add_to_list(dhead, node);
	sem_post(&dhead->left_len);
	sem_post(&dhead->sem);
}
#else //_LOCK_FREE_LIST

list_head * creat_list(int max_length)
{
	list_head * head;
	head = malloc(sizeof(list_head)+max_length*sizeof(void *));
	head->head = head->tail = 0;
	head->max_len = max_length;printf("lock free list\n");
	return head;
}
void add_to_list(list_head * head, void* node)
{
	int tail = head->tail;

	head->node[tail] = node;printf("add tail %d\n", tail);
	
	if(tail == head->max_len) tail = 0;
	else tail++;
	
	while(tail == head->head);
	head->tail = tail;
	return;
}
void* get_from_list(list_head * head)
{
	void * node = NULL;
	while(head->tail == head->head) ;printf("get head %d\n", head->head);
	node = head->node;
	if(head->head == head->max_len) head->head = 0;
	else head->head++;
	return node;
}

#endif
