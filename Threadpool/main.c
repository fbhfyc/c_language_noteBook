#include "threadpool.h"
#include <stdio.h>
#include <unistd.h>

// 示例任务函数
void print_number(void* arg) {
    int num = *(int*)arg;
    printf("Thread %lu: processing task %d\n", pthread_self(), num);
    sleep(1);  // 模拟耗时操作
}

int main() {
    ThreadPool* pool = ThreadPool_create(4, 10);  // 4个线程，队列大小10
    if (!pool) {
        fprintf(stderr, "Failed to create thread pool\n");
        return 1;
    }

    // 添加任务
    for (int i = 0; i < 20; i++) {
        int* num = malloc(sizeof(int));
        *num = i;
        if (ThreadPool_add_task(pool, print_number, num) != 0) {
            fprintf(stderr, "Failed to add task %d\n", i);
            free(num);
        }
    }

    // 等待所有任务完成
    sleep(5);  // 实际应用中应使用更优雅的方式等待

    // 销毁线程池
    ThreadPool_destroy(pool);
    return 0;
}
