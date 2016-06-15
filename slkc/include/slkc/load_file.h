#ifndef SLKC_LOAD_FILE_H
#define SLKC_LOAD_FILE_H

#include <slkc/memory.h>
#include <slkc/types.h>

void* sk_load_file(skAllocator allocator, const ascii_char* file_path, usize* file_size);

#endif /* !SLKC_LOAD_FILE_H */