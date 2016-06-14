#ifndef SLKC_MACROS_H
#define SLKC_MACROS_H

#include <assert.h>

#if _WIN32

#define SK_FORCEINLINE __forceinline
#define SK_NOINLiNE __declspec(noinline)
#define SK_ERROR(msg) assert(0 && msg);

#else
static_assert(0, "Unsupported target");
#endif

#endif /* !SLKC_MACROS_H */
