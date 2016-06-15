#include <slkc/load_file.h>
#include <stdio.h>
#include <string.h>

void* sk_load_file(skAllocator allocator, const ascii_char* file_path, usize* file_size)
{
	FILE* file = NULL;
	ascii_char* arr = NULL;
#if _WIN32
	fopen_s(&file, file_path, "rb");
#else
	file = fopen(file_path, "rb");
#endif
	assert(file != NULL);
	fseek(file, 0, SEEK_END);
	usize size = ftell(file);
	fseek(file, 0, SEEK_SET);
	arr = sk_mem_alloc(allocator, size + 1, 4);
	memset(arr, 0, size + 1);
	fread(arr, size, 1, file);
	fclose(file);
	*file_size = size;
	return (void*)arr;
}
