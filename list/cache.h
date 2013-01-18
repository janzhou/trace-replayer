#ifndef _H_TRANSFER_CACHE
#define _H_TRANSFER_CACHE

#include <semaphore.h>

typedef struct cache_head{
	void *next;
	void *pre;
	sem_t sem;
}cache_head;

cache_head * creat_cache(int num);
//hit the cache and move it to head
void * hit_cache(cache_head* head, void* data, int (* hit)(void* cache, void* data));
//remove the cache
void remove_cache(cache_head* head, void* data, int (* hit)(void* cache, void* data));
//add to head
void add_cache(cache_head* head, void* data);
//remove tail and add head
void swap_cache(cache_head* head, void* data);
//void trail_cache(cache_head * head);
void del_cache(cache_head * head);

#endif
