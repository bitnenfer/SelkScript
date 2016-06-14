#ifndef SLKC_TYPES_H
#define SLKC_TYPES_H

#include <inttypes.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef float float32;
typedef double float64;
typedef unsigned int usize;
typedef uintptr_t uintptr;
typedef intptr_t intptr;
typedef char ascii_char;

#ifndef __cplusplus
#define true 1
#define false 0
typedef uint8 bool;
#endif /* !__cplusplus */


#endif /* !SLKC_TYPES_H */
