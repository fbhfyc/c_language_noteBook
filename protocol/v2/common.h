// 在common.h中添加
#include <pthread.h>
#include <time.h>

#define MAX_QUEUE_SIZE 1000
#define WORKER_THREAD_NUM 4

// 消息结构
typedef struct {
    int client_fd;
    struct sockaddr_in client_addr;
    protocol_header_t packet;
    time_t receive_time;
} message_t;

// 线程安全消息队列
typedef struct {
    message_t* messages;
    int capacity;
    int size;
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} message_queue_t;

// 队列操作函数声明
int init_queue(message_queue_t* queue, int capacity);
void destroy_queue(message_queue_t* queue);
int enqueue_message(message_queue_t* queue, const message_t* msg);
int dequeue_message(message_queue_t* queue, message_t* msg);
