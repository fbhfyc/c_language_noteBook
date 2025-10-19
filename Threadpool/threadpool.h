#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

typedef struct {
    void (*function)(void*);  // 任务函数指针
    void* arg;                 // 任务参数
} Task;

typedef struct {
    Task* queue;               // 任务队列
    int queue_size;            // 队列大小
    int head;                  // 队首索引
    int tail;                  // 队尾索引
    int count;                 // 当前任务数
    pthread_mutex_t lock;      // 互斥锁
    pthread_cond_t notify;     // 条件变量
    pthread_t* threads;        // 工作线程数组
    int thread_count;          // 线程数量
    int shutdown;             // 关闭标志 (0:运行, 1:关闭)
} ThreadPool;

// 创建线程池
ThreadPool* ThreadPool_create(int thread_count, int queue_size);

// 添加任务
int ThreadPool_add_task(ThreadPool* pool, void (*function)(void*), void* arg);

// 销毁线程池
int ThreadPool_destroy(ThreadPool* pool);

#endif
