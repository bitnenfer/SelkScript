#ifndef SLKC_MEMORY_H
#define SLKC_MEMORY_H

#include <slkc/macros.h>
#include <slkc/types.h>

#define SK_SIZE_B1 1
#define SK_SIZE_B2 2
#define SK_SIZE_B4 4
#define SK_SIZE_B8 8
#define SK_SIZE_B16 16
#define SK_SIZE_B32 32
#define SK_SIZE_B64 64
#define SK_SIZE_B128 128
#define SK_SIZE_B256 256
#define SK_SIZE_B512 512
#define SK_SIZE_KB1 1024
#define SK_SIZE_KB2 2048
#define SK_SIZE_KB4 4096
#define SK_SIZE_KB8 8192
#define SK_SIZE_KB16 16384
#define SK_SIZE_KB32 32768
#define SK_SIZE_KB64 65536
#define SK_SIZE_KB128 131072
#define SK_SIZE_KB256 262144
#define SK_SIZE_KB512 524288
#define SK_SIZE_MB1 1048576
#define SK_SIZE_MB2 2097152
#define SK_SIZE_MB4 4194304
#define SK_SIZE_MB8 8388608
#define SK_SIZE_MB16 16777216
#define SK_SIZE_MB32 33554432
#define SK_SIZE_MB64 67108864
#define SK_SIZE_MB128 134217728
#define SK_SIZE_MB256 268435456
#define SK_SIZE_MB512 536870912
#define SK_SIZE_GB1 1073741824
#define SK_SIZE_GB2 2147483648

typedef struct skAllocator
{
	void*(*allocate_func)(void*, usize, usize);
	void(*deallocate_func)(void*, void*);
	void(*clear_func)(void*);
	void*(*realloc_func)(void*, void*, usize, usize, usize);
	void* allocator;
} skAllocator;

typedef struct skLinearAllocator
{
	void* buffer_head;
	void* buffer_tail;
	void* buffer_curr;
} skLinearAllocator;

typedef struct skMallocAllocator
{
	void* root_header;
	void* curr_header;
} skMallocAllocator;


/* Allocator Initialization */
void sk_linear_allocator_init(skLinearAllocator* allocator, void* buffer, usize buffer_size);
skAllocator sk_linear_allocator(skLinearAllocator* linear_allocator);
skAllocator sk_malloc_allocator(skMallocAllocator* malloc_allocator);
skAllocator sk_gmalloc();
void sk_init_gmalloc();
skAllocator sk_lmalloc();
void sk_init_gmalloc();
void sk_init_lmalloc(usize size);
void sk_init_memory(usize linear_size);
void sk_shutdown_memory();

/* Memory Managment API */
void* sk_mem_alloc(skAllocator allocator, usize size, usize alignment);
void sk_mem_free(skAllocator allocator, void* ptr);
void sk_mem_clear(skAllocator allocator);
void* sk_mem_realloc(skAllocator allocator, void* ptr, usize old_size, usize new_size, usize alignment);

/* Pointer Utils */
SK_FORCEINLINE void* sk_forward_ptr(void* p, size_t offset)
{
#if _DEBUG
	assert(p != NULL);
#endif
	return (void*)((char*)p + offset);
}

SK_FORCEINLINE void* sk_backward_ptr(void* p, size_t offset)
{
#if _DEBUG
	assert(p != NULL);
#endif
	return (void*)((char*)p - offset);
}

SK_FORCEINLINE void* sk_align_forward(void* p, size_t alignment)
{
#if _DEBUG
	assert(alignment != 0 && p != NULL);
#endif
	return (void*)(((uintptr)p + (uintptr)(alignment - 1)) & (uintptr)(~(alignment - 1)));
}

SK_FORCEINLINE bool sk_is_aligned(void* p, size_t alignment)
{
#if _DEBUG
	assert(alignment != 0 && p != NULL);
#endif
	return (uintptr)p % alignment == 0;
}

SK_FORCEINLINE bool sk_in_range(void* ptr, const void* start, const void* end)
{
	return ptr >= start && ptr < end;
}
#endif /* !SLKC_MEMORY_H */
