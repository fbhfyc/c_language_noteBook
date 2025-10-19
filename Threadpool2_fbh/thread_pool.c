#include "thread_pool.h"
#include <stdlib.h>
#include <stdio.h>

static void* worker(void* arg) 
{
	ThreadPool* pool = (ThreadPool*)arg;
	while ( 1 ) {
		pthread_mutex_lock(&pool->lock);

		while (pool->count==0 && !pool->shutdown) {
			pthread_cond_wait(&pool->notify, &pool->lock);
		}
		if ( pool->shutdown ) {
			pthread_mutex_unlock(&pool->lock);
			pthread_exit(NULL);
		}

		Task task = pool->queue[pool->head];
		pool->head = ( pool->head +1 ) % pool->queue_size;
		pool->count--;
		pthread_mutex_unlock(&pool->lock);
		(task.fun)(task.arg);
		
	}		
	return NULL;
}

ThreadPool* ThreaadPool_create(int thread_count, int queue_size)
{
	ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
	if ( !pool ) return NULL;

	pool->queue = (Task*)malloc(sizeof(Task) * queue_size);
	if ( pool->queue ) return NULL;
	pool->queue_size = queue_size;
	pool->head = pool->tail = pool->queue_size = 0;

	pool->threads = (pthread_t*)malloc(sizeof(pthread_t) * thread_count);
	pool->thread_count = thread_count;
	pool->shutdown = 0;

	pthread_mutex_init(&pool->lock,NULL);
	pthread_cond_init(&pool->notify,NULL);

	for (int i=0; i<thread_count; i++) {
	        int ret = pthread_create(&pool->threads[i], NULL, worker, pool);
		if (ret != 0) {
			ThreadPool_destroy(pool);
			return NULL;
		}
	}
	return pool;
}

int ThreadPool_add_task(ThreadPool* pool, void (*fun)(void*), void* arg)
{
	pthread_mutex_lock(&pool->lock);
	if ( pool->count == pool->queue_size ) {
	   pthread_mutex_unlock(&pool->lock);
	   printf("task queue full");
	   return -1;
	}
	pool->queue[pool->tail].fun = fun;
	pool->queue[pool->tail].arg = arg;
	pool->tail = (pool->tail + 1) % pool->queue_size;
	pool->count++;

	pthread_cond_signal(&pool->notify);
	pthread_mutex_unlock(&pool->lock);
	return 0;
}

int ThreadPool_destroy(ThreadPool* pool)
{
	if ( !pool->shutdown )  return -1; 

	while ( 0 != pool->count ) {
		pthread_cond_signal(&pool->notify);
	}
	pthread_mutex_destroy(&pool->lock);
	pthread_cond_destroy(&pool->notify);
	free(pool->queue);
	free(pool);

}
