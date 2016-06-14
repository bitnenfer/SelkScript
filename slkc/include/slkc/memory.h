#ifndef SLKC_MEMORY_H
#define SLKC_MEMORY_H

#include <slkc/macros.h>
#include <slkc/types.h>

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
