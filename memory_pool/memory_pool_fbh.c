#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

typedef struct {
	void** freelist;
	size_t block_size;
	size_t num_blocks;
	size_t free_counts;
	void*  memory;
	pthread_mutex_t mutex;
}MemoryPool;

MemoryPool* mp_creat(size_t block_size, size_t num_blocks)
{
	size_t total_memory_size = block_size * num_blocks;
	size_t free_list_size = (sizeof(void*)) * num_blocks;

	MemoryPool* pool = (MemoryPool*)malloc(sizeof(MemoryPool));
	if (!pool) return NULL;

	pool->block_size = block_size;
	pool->num_blocks = num_blocks;
	pool->free_counts = num_blocks;
	pool->memory = malloc(total_memory_size + free_list_size);
	if (!pool->memory) {free(pool); return NULL;}

	pool->freelist = (void**)((char*)pool->memory + total_memory_size);

	for ( size_t i=0; i<num_blocks; i++ ) {
		pool->freelist[i] = (char*)pool->memory + (block_size* i);

	}
	
	pthread_mutex_init(&pool->mutex,NULL);
	return pool;
}

void* mp_alloc(MemoryPool* pool)
{
	if ( !pool || 0 == pool->free_counts ) {
		printf("there is no free block");
		return NULL;
	}

	pthread_mutex_lock(&pool->mutex);

	void* block = pool->freelist[pool->free_counts - 1];
	pool->free_counts--;

	pthread_mutex_unlock(&pool->mutex);

	memset(block, 0, sizeof(pool->block_size));

	return block; 
}

int  mp_free(MemoryPool* pool, void* block)
{
	if ( !pool || !block ) {
		return NULL;
	}



	pthread_mutex_lock(&pool->mutex);

	pthread_mutex_unlock(&pool->mutex);

	return 0;
}

void mp_getStatus(MemoryPool* pool)
{
}

int mp_destroy(MemoryPool* pool)
{
	return 0;
}

int main()
{
	return 0;
}
