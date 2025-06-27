#pragma once

/** @file lake/bedrock/machina.h 
 *  @brief Allocator for cache-friendly and fiber-aware general purpose allocations.
 *
 *  TODO docs
 *  Slab-based? Preferably lockless.
 */
#include <lake/bedrock/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* rename them, exploit compiler optimizations */
#define lake_memset     memset
#define lake_memcpy     memcpy
#define lake_memcmp     memcmp
#define lake_memrchr    memrchr
#define lake_memmove    memmove
#define lake_strlen     strlen
#define lake_strncmp    strncmp
#define lake_strncpy    strncpy
#define lake_strchrnul  strchrnul
#define lake_strchr     strchr
#define lake_strrchr    strrchr

#define lake_zero(mem)  lake_memset(&(mem), 0, sizeof((mem)))
#define lake_zerop(mem) lake_memset((mem), 0, sizeof(*(mem)))
#define lake_zeroa(mem) lake_memset((mem), 0, sizeof((mem)))

/** Aligned OS allocator, used temporarily until i implement other strategies 
 *  for memory allocation, and how to integrate them with my fiber scheduler.
 *  They will later be replaced, and code will be refactored. TODO */
LAKEAPI LAKE_HOT_FN 
void *LAKECALL __lake_malloc(usize size, usize align);

#define __lake_malloc_t(T)    lake_reinterpret_cast(T *, __lake_malloc(sizeof(T), alignof(T)))
#define __lake_malloc_n(T, n) lake_reinterpret_cast(T *, __lake_malloc(sizeof(T) * (n), alignof(T)))

/** Reallocates memory allocated from `__lake_malloc()`. */
LAKEAPI LAKE_HOT_FN 
void *LAKECALL __lake_realloc(void *ptr, usize size, usize align);

#define __lake_realloc_t(ptr, T)    __lake_realloc(ptr, sizeof(T), alignof(T))
#define __lake_realloc_n(ptr, T, n) __lake_realloc(ptr, sizeof(T) * (n), alignof(T))

/** Frees memory allocated from `__lake_malloc()`. */
LAKEAPI LAKE_HOT_FN 
void LAKECALL __lake_free(void *ptr);

/* TODO machina */

#ifdef __cplusplus
}
#endif /* __cplusplus */
