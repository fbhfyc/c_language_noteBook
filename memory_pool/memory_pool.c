#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// 内存池结构体
typedef struct {
    void** free_blocks;       // 空闲块链表
    size_t block_size;        // 每个块的大小
    size_t num_blocks;        // 总块数量
    size_t free_count;        // 空闲块数量
    void* memory;             // 内存池起始地址
    pthread_mutex_t mutex;    // 互斥锁，支持多线程
} MemoryPool;

// 创建内存池
MemoryPool* mp_create(size_t block_size, size_t num_blocks) {
    // 计算内存池总大小
    size_t total_memory_size = block_size * num_blocks;
    size_t free_list_size = sizeof(void*) * num_blocks;
    
    // 分配内存池控制结构
    MemoryPool* pool = (MemoryPool*)malloc(sizeof(MemoryPool));
    if (!pool) return NULL;
    
    // 分配内存池数据区域和空闲链表
    pool->memory = malloc(total_memory_size + free_list_size);
    if (!pool->memory) {
        free(pool);
        return NULL;
    }
    
    // 初始化成员变量
    pool->block_size = block_size;
    pool->num_blocks = num_blocks;
    pool->free_count = num_blocks;
    pool->free_blocks = (void**)((char*)pool->memory + total_memory_size);
    
    // 初始化空闲块链表
    for (size_t i = 0; i < num_blocks; i++) {
        pool->free_blocks[i] = (char*)pool->memory + (i * block_size);
    }
    
    // 初始化互斥锁
    pthread_mutex_init(&pool->mutex, NULL);
    
    return pool;
}

// 从内存池分配一个块
void* mp_alloc(MemoryPool* pool) {
    if (!pool || pool->free_count == 0) {
        return NULL; // 内存池为空或没有空闲块
    }
    
    pthread_mutex_lock(&pool->mutex);
    
    // 获取最后一个空闲块
    void* block = pool->free_blocks[pool->free_count - 1];
    pool->free_count--;
    
    pthread_mutex_unlock(&pool->mutex);
    
    // 清空分配的内存，避免数据残留
    memset(block, 0, pool->block_size);
    
    return block;
}

// 释放块到内存池
int mp_free(MemoryPool* pool, void* block) {
    if (!pool || !block) {
        return -1; // 参数无效
    }
    
    // 检查块是否属于这个内存池
    if (block < pool->memory || 
        block >= (char*)pool->memory + pool->block_size * pool->num_blocks) {
        return -2; // 块不属于此内存池
    }
    
    pthread_mutex_lock(&pool->mutex);
    
    if (pool->free_count >= pool->num_blocks) {
        pthread_mutex_unlock(&pool->mutex);
        return -3; // 内存池已满
    }
    
    // 将块添加到空闲链表
    pool->free_blocks[pool->free_count] = block;
    pool->free_count++;
    
    pthread_mutex_unlock(&pool->mutex);
    
    return 0; // 成功释放
}

// 销毁内存池
void mp_destroy(MemoryPool* pool) {
    if (!pool) return;
    
    free(pool->memory);
    pthread_mutex_destroy(&pool->mutex);
    free(pool);
}

// 打印内存池状态
void mp_print_status(MemoryPool* pool) {
    if (!pool) return;
    
    printf("内存池状态: \n");
    printf("  块大小: %zu 字节\n", pool->block_size);
    printf("  总块数: %zu\n", pool->num_blocks);
    printf("  空闲块数: %zu\n", pool->free_count);
    printf("  使用率: %.2f%%\n", 
           (1.0 - (double)pool->free_count / pool->num_blocks) * 100);
}

// 示例用法
int main() {
    // 创建一个内存池：每个块32字节，共10个块
    MemoryPool* pool = mp_create(32, 10);
    if (!pool) {
        printf("创建内存池失败!\n");
        return 1;
    }
    
    mp_print_status(pool);
    
    // 分配一些块
    void* blocks[5];
    for (int i = 0; i < 5; i++) {
        blocks[i] = mp_alloc(pool);
        if (blocks[i]) {
            printf("分配块 %d 成功\n", i);
            // 写入一些测试数据
            sprintf((char*)blocks[i], "这是第 %d 个块的数据", i);
        } else {
            printf("分配块 %d 失败\n", i);
        }
    }
    
    mp_print_status(pool);
    
    // 打印块中的数据
    for (int i = 0; i < 5; i++) {
        if (blocks[i]) {
            printf("块 %d 中的数据: %s\n", i, (char*)blocks[i]);
        }
    }
    
    // 释放一些块
    for (int i = 0; i < 3; i++) {
        int ret = mp_free(pool, blocks[i]);
        if (ret == 0) {
            printf("释放块 %d 成功\n", i);
        } else {
            printf("释放块 %d 失败，错误码: %d\n", i, ret);
        }
    }
    
    mp_print_status(pool);
    
    // 销毁内存池
    mp_destroy(pool);
    printf("内存池已销毁\n");
    
    return 0;
}
    
