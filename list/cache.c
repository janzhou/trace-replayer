#include <malloc.h>
#include "cache.h"

typedef struct cache_node{
	struct cache_node *next;
	struct cache_node *pre;
	void * data;
}cache_node;

cache_head * creat_cache(int num)
{
	cache_head* head = malloc(sizeof(cache_head));
	cache_node* node = NULL;

	if(head){
		head->next = head->pre = head;
		sem_init(&head->sem, 0, 1);
	}
	while(num--){
		node = malloc(sizeof(cache_node));
		node->data = NULL;

		//add to head
		node->next = (cache_node*)head->next;
		node->pre = (cache_node*)head;
		((cache_node*)head->next)->pre = node;
		head->next = node;
	}

	return head;
}

void * hit_cache(cache_head* head, void* data, int (* hit)(void* cache, void* data))
{
	cache_node* node = NULL;
	void * ret = NULL;
	sem_wait(&head->sem);
	node = head->next;
	while(node != (cache_node*)head && node->data){
		if(!(* hit)(node->data, data)){
			ret = node->data;
			if(node == head->next) 
				break;

			//detach
			node->next->pre = node->pre;
			node->pre->next = node->next;

			//add to head
			node->next = (cache_node*)head->next;
			node->pre = (cache_node*)head;
			((cache_node*)head->next)->pre = node;
			head->next = node;
			break;
		}
		node = node->next;
	}
	sem_post(&head->sem);
	return ret;
}

void remove_cache(cache_head* head, void* data, int (* hit)(void* cache, void* data))
{
	cache_node* node = NULL;
	sem_wait(&head->sem);
	node = head->next;
	while(node != (cache_node*)head && node->data){
		if(!(* hit)(node->data, data)){
			//detach
			node->next->pre = node->pre;
			node->pre->next = node->next;

		//	free(node->data);
			free(node);
			break;
		}
		node = node->next;
	}
	sem_post(&head->sem);
	return;
}

void add_cache(cache_head* head, void* data)
{
	cache_node* node;
	if(!head) return;
	sem_wait(&head->sem);

	node = malloc(sizeof(cache_node));
	node->data = data;

	//add to head
	node->next = (cache_node*)head->next;
	node->pre = (cache_node*)head;
	((cache_node*)head->next)->pre = node;
	head->next = node;
	sem_post(&head->sem);
}
int get_len_cache(cache_head * head, int* plen)
{
	return sem_getvalue(&head->sem, plen);
}

void swap_cache(cache_head* head, void* data)
{
	cache_node* node;
	if(!head) return;
	sem_wait(&head->sem);

	if(head->next == head) return;

	//detach tail
	node = head->pre;
	node->pre->next = (cache_node*)head;
	head->pre = node->pre;

	if(node->data) free(node->data);
	node->data = data;

	//add to head
	node->next = (cache_node*)head->next;
	node->pre = (cache_node*)head;
	((cache_node*)head->next)->pre = node;
	head->next = node;

	sem_post(&head->sem);
}
/*
void trail_cache(cache_head * head)
{
	cache_node* node;
	if(!head) return;
	sem_wait(&head->sem);
	if(head->next == head) return;
	node = head->pre;
	node->pre->next = head;
	head->pre = node->pre
	free(node->data);
	free(node);
	sem_post(&head->sem);
}
*/
void del_cache(cache_head * head)
{
	if(!head) return;
	sem_destroy(&head->sem);
	free(head);
}
