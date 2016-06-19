#include <slkc/array.h>
#include <slkc/memory.h>


usize sk_array_length(void* raw_array) {
	skArrayHeader* header = sk_backward_ptr(raw_array, sizeof(skArrayHeader));
	return header->item_length;
}

usize sk_array_capacity(void* raw_array) {
	skArrayHeader* header = sk_backward_ptr(raw_array, sizeof(skArrayHeader));
	return header->item_capacity;
}

void sk_set_array_header(skArrayHeader* header, usize item_size, usize real_capacity, usize item_capacity, usize alignment, void* native_ptr, skAllocator allocator) {
	header->item_length = 0;
	header->real_length = 0;
	header->item_size = item_size;
	header->real_capacity = real_capacity;
	header->item_capacity = item_capacity;
	header->alignment = alignment;
	header->native_ptr = native_ptr;
	header->allocator = allocator;
}

void* sk_alloc_array(skAllocator allocator, usize capacity, usize alignment, usize item_capacity, usize item_size) {
	void* alloc_ptr = sk_mem_alloc(allocator, capacity + sizeof(skArrayHeader), alignment);
	void* return_ptr = sk_align_forward(sk_forward_ptr(alloc_ptr, sizeof(skArrayHeader)), alignment);

	sk_set_array_header(sk_array_get_header(return_ptr), item_size, capacity, item_capacity, alignment, alloc_ptr, allocator);
	return return_ptr;
}

skArrayHeader* sk_array_get_header(void* raw_array) {
	return ((skArrayHeader*)sk_backward_ptr(raw_array, sizeof(skArrayHeader)));
}
