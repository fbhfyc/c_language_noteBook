#include "common.h"

// 初始化队列
int init_queue(message_queue_t* queue, int capacity) {
    queue->messages = (message_t*)malloc(capacity * sizeof(message_t));
    if (!queue->messages) return -1;
    
    queue->capacity = capacity;
    queue->size = 0;
    queue->front = 0;
    queue->rear = 0;
    
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
    
    return 0;
}

// 销毁队列
void destroy_queue(message_queue_t* queue) {
    if (queue->messages) {
        free(queue->messages);
        queue->messages = NULL;
    }
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
}

// 入队
int enqueue_message(message_queue_t* queue, const message_t* msg) {
    pthread_mutex_lock(&queue->mutex);
    
    while (queue->size >= queue->capacity) {
        // 队列满，等待
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    }
    
    queue->messages[queue->rear] = *msg;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;
    
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    
    return 0;
}

// 出队
int dequeue_message(message_queue_t* queue, message_t* msg) {
    pthread_mutex_lock(&queue->mutex);
    
    while (queue->size <= 0) {
        // 队列空，等待
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }
    
    *msg = queue->messages[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);
    
    return 0;
}
