#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// 共享资源：一个简单的计数器
int shared_counter = 0;

// 读写锁
pthread_rwlock_t rwlock;

// 读者线程函数：读取共享计数器的值
void *reader(void *arg) {
    int id = *(int *)arg;
    free(arg); // 释放动态分配的ID
    
    while (1) {
        // 获取读锁
        if (pthread_rwlock_rdlock(&rwlock) != 0) {
            perror("pthread_rwlock_rdlock failed");
            pthread_exit(NULL);
        }
        
        // 读取共享资源
        printf("读者 %d: 读取到计数器值 = %d\n", id, shared_counter);
        
        // 模拟读取操作耗时
        usleep(rand() % 100000); // 0-100ms
        
        // 释放读锁
        if (pthread_rwlock_unlock(&rwlock) != 0) {
            perror("pthread_rwlock_unlock failed");
            pthread_exit(NULL);
        }
        
        // 读者休息一段时间时间再继续读取
        usleep(rand() % 500000); // 0-500ms
    }
    
    pthread_exit(NULL);
}

// 写者线程函数：修改共享计数器的值
void *writer(void *arg) {
    int id = *(int *)arg;
    free(arg); // 释放动态分配的ID
    
    while (1) {
        // 获取写锁
        if (pthread_rwlock_wrlock(&rwlock) != 0) {
            perror("pthread_rwlock_wrlock failed");
            pthread_exit(NULL);
        }
        
        // 修改共享资源
        int old_value = shared_counter;
        shared_counter++;
        printf("===== 写者 %d: 将计数器从 %d 修改为 %d =====\n", 
               id, old_value, shared_counter);
        
        // 模拟写入操作耗时
        usleep(rand() % 200000); // 0-200ms
        
        // 释放写锁
        if (pthread_rwlock_unlock(&rwlock) != 0) {
            perror("pthread_rwlock_unlock failed");
            pthread_exit(NULL);
        }
        
        // 写者休息一段时间再继续写入
        usleep(rand() % 1000000); // 0-1000ms
    }
    
    pthread_exit(NULL);
}

int main() {
    pthread_t readers[3], writers[2];
    int i, *id;
    
    // 初始化随机数种子
    srand(time(NULL));
    
    // 初始化读写锁
    if (pthread_rwlock_init(&rwlock, NULL) != 0) {
        perror("pthread_rwlock_init failed");
        exit(EXIT_FAILURE);
    }
    
    // 创建3个读者线程
    for (i = 0; i < 3; i++) {
        id = malloc(sizeof(int));
        *id = i + 1; // 读者ID从1开始
        if (pthread_create(&readers[i], NULL, reader, id) != 0) {
            perror("pthread_create failed for reader");
            exit(EXIT_FAILURE);
        }
    }
    
    // 创建2个写者线程
    for (i = 0; i < 2; i++) {
        id = malloc(sizeof(int));
        *id = i + 1; // 写者ID从1开始
        if (pthread_create(&writers[i], NULL, writer, id) != 0) {
            perror("pthread_create failed for writer");
            exit(EXIT_FAILURE);
        }
    }
    
    // 让主线程运行一段时间后退出
    sleep(10);
    printf("\n主线程：程序运行结束\n");
    
    // 销毁读写锁
    pthread_rwlock_destroy(&rwlock);
    
    return 0;
}
    
