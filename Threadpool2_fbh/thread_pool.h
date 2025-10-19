#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <pthread.h>

typedef struct {
	void (*fun)(void*);
	void* arg;
}Task;

typedef struct {
	int             shutdown;
	pthread_mutex_t lock;
	pthread_cond_t  notify;
	pthread_t*      threads;
	int             thread_count;
	Task*           queue;
	int             queue_size;
	int             head;
	int             tail;
        int             count;
}ThreadPool;

ThreadPool* ThreadPool_create(int thread_count, int queue_size);

int ThreadPool_add_task(ThreadPool* pool, void (*fun)(void*), void* arg);

int ThreadPool_destroy(ThreadPool* pool);


#endif

