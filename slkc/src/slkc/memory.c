#include <slkc/memory.h>
#include <xmmintrin.h>
#include <string.h>
#include <stdio.h>

void* sk_forward_ptr(void* p, size_t offset);
void* sk_backward_ptr(void* p, size_t offset);
void* sk_align_forward(void* p, size_t alignment);
bool sk_is_aligned(void* p, size_t alignment);
bool sk_in_range(void* ptr, const void* start, const void* end);
void* linear_malloc(void* allocator, usize size, usize alignment);
void linear_free(void* allocator, void* ptr);
void linear_clear(void* allocator);
void* malloc_malloc(void* allocator, usize size, usize alignment);
void malloc_free(void* allocator, void* ptr);
void malloc_clear(void* allocator);

/* Global Malloc Allocator */
static bool global_malloc_inited = false;
static skMallocAllocator global_malloc_struct;
static skAllocator global_malloc_allocator;

/* Global Linear Allocator */
static bool global_linear_inited = false;
static skLinearAllocator global_linear_struct;
static skAllocator global_linear_allocator;

#define SK_SENTINEL_TAG 0xDEAD
typedef struct skMallocHeader
{
	uint16 sentinel_tag;
	usize alloc_size;
	struct skMallocHeader* prev;
	struct skMallocHeader* next;
	void* native_ptr;
} skMallocHeader;

SK_FORCEINLINE void* malloc_malloc(void* allocator, usize size, usize alignment)
{
	void* alloc_ptr = NULL;
	void* retrn_ptr = NULL;
	skMallocHeader* header = NULL;
	skMallocAllocator* malloc_alloc = NULL;
	malloc_alloc = (skMallocAllocator*)allocator;
	alloc_ptr = malloc(size + sizeof(skMallocHeader));
	retrn_ptr = sk_align_forward(sk_forward_ptr(alloc_ptr, sizeof(skMallocHeader)), alignment);
	header = (skMallocHeader*)sk_backward_ptr(retrn_ptr, sizeof(skMallocHeader));
	header->next = NULL;
	header->prev = NULL;
	header->native_ptr = alloc_ptr;
	header->alloc_size = size;
	header->sentinel_tag = SK_SENTINEL_TAG;
	if (malloc_alloc->root_header == NULL)
	{
		malloc_alloc->root_header = (void*)header;
		malloc_alloc->curr_header = (void*)header;
	}
	else
	{
		skMallocHeader* last_header = malloc_alloc->curr_header;
		header->prev = last_header;
		header->next = NULL;
		last_header->next = header;
	}
	return retrn_ptr;
}

SK_FORCEINLINE void* malloc_realloc(void* allocator, void* ptr, usize old_size, usize new_size, usize alignment)
{
	if (old_size < new_size)
	{
		void* new_ptr = malloc_malloc(allocator, new_size, alignment);
		new_ptr = memcpy(new_ptr, ptr, old_size);
		malloc_free(allocator, ptr);
		return new_ptr;
	}
	return ptr;
}

SK_FORCEINLINE void malloc_free(void* allocator, void* ptr)
{
	skMallocHeader* header = NULL;
	skMallocAllocator* malloc_alloc = NULL;
	malloc_alloc = (skMallocAllocator*)allocator;
	header = (skMallocHeader*)sk_backward_ptr(ptr, sizeof(skMallocHeader));
	if (header->sentinel_tag != SK_SENTINEL_TAG)
		SK_ERROR("Corrupted header or block not allocated by malloc allocator.\n");
	skMallocHeader* prev = header->prev;
	skMallocHeader* next = header->next;
	if (prev != NULL)
		prev->next = next;
	if (next != NULL)
		next->prev = prev;
	if (header == malloc_alloc->curr_header && header != malloc_alloc->root_header)
	{
		malloc_alloc->curr_header = header->prev;
	}
	free(header->native_ptr);
}

SK_FORCEINLINE void malloc_clear(void* allocator)
{
	skMallocAllocator* malloc_alloc = (skMallocAllocator*)allocator;
	skMallocHeader* root_header = malloc_alloc->root_header;
	skMallocHeader* curr_header = root_header;
	skMallocHeader* next_header = NULL;
	while (curr_header != NULL)
	{
		if (curr_header->sentinel_tag != SK_SENTINEL_TAG)
			SK_ERROR("Corrupted header or block not allocated by malloc allocator.\n");

		next_header = curr_header->next;
		free(curr_header->native_ptr);
		curr_header = next_header;
	}
}

SK_FORCEINLINE void* linear_malloc(void* allocator, usize size, usize alignment)
{
	skLinearAllocator* linear_alloc = (skLinearAllocator*)allocator;
	void* old_curr = sk_align_forward(linear_alloc->buffer_curr, alignment);
	void* next_curr = sk_forward_ptr(old_curr, size);
#if _DEBUG
	assert(sk_in_range(next_curr, linear_alloc->buffer_head, linear_alloc->buffer_tail));
	linear_alloc->buffer_curr = next_curr;
	return old_curr;
#else
	linear_alloc->buffer_curr = next_curr;
	return old_curr;
#endif
}

SK_FORCEINLINE void* linear_realloc(void* allocator, void* ptr, usize old_size, usize new_size, usize alignment)
{
	if (old_size < new_size)
	{
		void* new_ptr = linear_malloc(allocator, new_size, alignment);
		memmove(new_ptr, ptr, old_size);
		return new_ptr;
	}
	return ptr;
}

SK_FORCEINLINE void linear_free(void* allocator, void* ptr)
{
	//SK_ERROR("Can't free memory on linear allocator.\n");
}

SK_FORCEINLINE void linear_clear(void* allocator)
{
	skLinearAllocator* linear_alloc = (skLinearAllocator*)allocator;
	linear_alloc->buffer_curr = linear_alloc->buffer_head;
}

void sk_linear_allocator_init(skLinearAllocator* allocator, void* buffer, usize buffer_size)
{
	allocator->buffer_head = buffer;
	allocator->buffer_curr = buffer;
	allocator->buffer_tail = sk_forward_ptr(buffer, buffer_size);
}

skAllocator sk_linear_allocator(skLinearAllocator* linear_allocator)
{
	skAllocator allocator;
	allocator.allocate_func = &linear_malloc;
	allocator.deallocate_func = &linear_free;
	allocator.clear_func = &linear_clear;
	allocator.realloc_func = &linear_realloc;
	allocator.allocator = (void*)linear_allocator;
	return allocator;
}

skAllocator sk_malloc_allocator(skMallocAllocator* malloc_allocator)
{
	skAllocator allocator;
	allocator.allocator = (void*)malloc_allocator;
	allocator.allocate_func = &malloc_malloc;
	allocator.deallocate_func = &malloc_free;
	allocator.clear_func = &malloc_clear;
	allocator.realloc_func = &malloc_realloc;
	malloc_allocator->curr_header = NULL;
	malloc_allocator->root_header = NULL;
	return allocator;
}

skAllocator sk_gmalloc()
{
	return global_malloc_allocator;
}

skAllocator sk_lmalloc()
{
	return global_linear_allocator;
}

void sk_init_gmalloc()
{
	if (global_malloc_inited) return;
	global_malloc_allocator = sk_malloc_allocator(&global_malloc_struct);
	global_malloc_inited = true;
}

void sk_init_lmalloc(usize size)
{
	if (global_linear_inited) return;
	sk_init_gmalloc();
	sk_linear_allocator_init(&global_linear_struct, sk_mem_alloc(sk_gmalloc(), size, 4), size);
	global_linear_allocator = sk_linear_allocator(&global_linear_struct);
	global_linear_inited = true;
}

void sk_init_memory(usize linear_size)
{
	sk_init_gmalloc();
	sk_init_lmalloc(linear_size);
}

void sk_shutdown_memory()
{
	sk_mem_clear(global_linear_allocator);
	sk_mem_clear(global_malloc_allocator);
}
/* --- */

void* sk_mem_alloc(skAllocator allocator, usize size, usize alignment)
{
	return allocator.allocate_func(allocator.allocator, size, alignment);
}

void sk_mem_free(skAllocator allocator, void* ptr)
{
	allocator.deallocate_func(allocator.allocator, ptr);
}

void sk_mem_clear(skAllocator allocator)
{
	allocator.clear_func(allocator.allocator);
}

void* sk_mem_realloc(skAllocator allocator, void* ptr, usize old_size, usize new_size, usize alignment)
{
	return allocator.realloc_func(allocator.allocator, ptr, old_size, new_size, alignment);
}


