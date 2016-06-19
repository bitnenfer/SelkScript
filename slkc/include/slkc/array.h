
#ifndef SLKC_ARRAY_H
#define SLKC_ARRAY_H

#include <slkc/types.h>
#include <slkc/memory.h>
#include <string.h>

typedef struct skArrayHeader {
	skAllocator allocator;
	void* native_ptr;
	usize real_length;
	usize item_length;
	usize real_capacity;
	usize item_capacity;
	usize item_size;
	usize alignment;
} skArrayHeader;

usize sk_array_length(void* raw_array);
usize sk_array_capacity(void* raw_array);
void* sk_alloc_array(skAllocator allocator, usize capacity, usize alignment, usize item_capacity, usize item_size);
void* sk_alloc_array(skAllocator allocator, usize capacity, usize alignment, usize item_capacity, usize item_size);
skArrayHeader* sk_array_get_header(void* raw_array);

#define sk_array(allocator, type, capacity, alignment) ((type*)sk_alloc_array(allocator, capacity * sizeof(type), alignment, capacity, sizeof(type)))
#define sk_array_tail(raw_array) (raw_array[sk_array_get_header(raw_array)->item_length - 1])
#define sk_array_alloc(raw_array, item_count) { \
	skArrayHeader* header = sk_array_get_header(raw_array); \
	if (header->item_length + item_count >= header->item_capacity) \
	{ \
		void* new_array = sk_alloc_array(header->allocator, header->real_capacity * 2, header->alignment, header->item_capacity * 2, header->item_size); \
		usize real_length = header->real_length; \
		usize item_length = header->item_length; \
		memmove(new_array, raw_array, header->real_capacity); \
		sk_mem_free(header->allocator, header->native_ptr); \
		raw_array = new_array; \
		header = sk_array_get_header(raw_array); \
		header->real_length = real_length; \
		header->item_length = item_length; \
	} \
	header->real_length += header->item_size * item_count; \
	header->item_length += item_count; \
}
#define sk_array_push(raw_array, item) { \
	skArrayHeader* header = sk_array_get_header(raw_array); \
	assert(sizeof(item) == header->item_size); \
	if (header->item_length >= header->item_capacity) \
	{ \
		void* new_array = sk_alloc_array(header->allocator, header->real_capacity * 2, header->alignment, header->item_capacity * 2, sizeof(item)); \
		usize real_length = header->real_length; \
		usize item_length = header->item_length; \
		memmove(new_array, raw_array, header->real_capacity); \
		sk_mem_free(header->allocator, header->native_ptr); \
		raw_array = new_array; \
		header = sk_array_get_header(raw_array); \
		header->real_length = real_length; \
		header->item_length = item_length; \
	} \
	raw_array[header->item_length] = item; \
	header->real_length += sizeof(item); \
	header->item_length += 1; \
}

#define sk_array_clear(raw_array) { \
	skArrayHeader* header = sk_array_get_header(raw_array); \
	if (header->real_length > 0) { \
		header->real_length = 0; \
		header->item_length = 0; \
	} \
}

#define sk_array_free(raw_array) { \
	skArrayHeader* header = sk_array_get_header(raw_array); \
	sk_mem_free(header->allocator, header->native_ptr); \
}

#define sk_array_pop(raw_array) { \
	skArrayHeader* header = sk_array_get_header(raw_array); \
	if (header->real_length > 0) { \
		header->real_length -= header->item_size; \
		header->item_length -= 1; \
	} \
}

#endif /* !SLKC_ARRAY_H */