#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// 工作线程函数
static void* worker(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    while (1) {
        pthread_mutex_lock(&pool->lock);
        
        // 等待任务或关闭信号
        while (pool->count == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->notify, &pool->lock);
        }

        // 检查是否关闭
        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->lock);
            pthread_exit(NULL);
        }

        // 取任务
        Task task = pool->queue[pool->head];
        pool->head = (pool->head + 1) % pool->queue_size;
        pool->count--;
        
        pthread_mutex_unlock(&pool->lock);
        
        // 执行任务
        (task.function)(task.arg);
    }
    return NULL;
}

// 创建线程池
ThreadPool* ThreadPool_create(int thread_count, int queue_size) {
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    if (!pool) return NULL;

    // 初始化任务队列
    pool->queue = (Task*)malloc(sizeof(Task) * queue_size);
    pool->queue_size = queue_size;
    pool->head = pool->tail = pool->count = 0;

    // 初始化线程
    pool->threads = (pthread_t*)malloc(sizeof(pthread_t) * thread_count);
    pool->thread_count = thread_count;
    pool->shutdown = 0;

    // 初始化锁和条件变量
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->notify, NULL);

    // 创建工作线程
    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&pool->threads[i], NULL, worker, pool) != 0) {
            ThreadPool_destroy(pool);
            return NULL;
        }
    }
    return pool;
}

// 添加任务
int ThreadPool_add_task(ThreadPool* pool, void (*function)(void*), void* arg) {
    
    for (int i=0; i<5; i++) {
        pthread_mutex_lock(&pool->lock);
        if (pool->count == pool->queue_size) {
            pthread_mutex_unlock(&pool->lock);
            pthread_cond_signal(&pool->notify);// 唤醒一个线程
	    sleep(1);
            if ( i==4 )  return -1;  // 队列已满
        } else {
		break;
	}
    }

    // 添加任务到队尾
    pool->queue[pool->tail].function = function;
    pool->queue[pool->tail].arg = arg;
    pool->tail = (pool->tail + 1) % pool->queue_size;
    pool->count++;
    
    pthread_cond_signal(&pool->notify);  // 唤醒一个线程
    pthread_mutex_unlock(&pool->lock);
    return 0;
}

// 销毁线程池
int ThreadPool_destroy(ThreadPool* pool) {
    if (!pool) return -1;

    pthread_mutex_lock(&pool->lock);
    pool->shutdown = 1;
    pthread_cond_broadcast(&pool->notify);  // 唤醒所有线程
    pthread_mutex_unlock(&pool->lock);

    // 等待所有线程退出
    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    // 释放资源
    free(pool->queue);
    free(pool->threads);
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->notify);
    free(pool);
    return 0;
}

