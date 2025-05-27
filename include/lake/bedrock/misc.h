#pragma once

#include <lake/bedrock/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Sets first N bytes of a memory region to `c` per byte. */
LAKE_HOT_FN LAKE_NONNULL(1) 
LAKEAPI void *LAKECALL 
lake_memset(
    void *restrict dst, 
    s32            c, 
    usize          n);

#define lake_zero(mem)  lake_memset(&(mem), 0, sizeof((mem)))
#define lake_zerop(mem) lake_memset((mem), 0, sizeof(*(mem)))
#define lake_zeroa(mem) lake_memset((mem), 0, sizeof((mem)))

/** Copies first N bytes from source region to destination region. */
LAKE_HOT_FN LAKE_NONNULL(1,2) 
LAKEAPI void *LAKECALL 
lake_memcpy(
    void       *restrict dst, 
    void const *restrict src, 
    usize                n);

/** Compares the first N bytes of two memory regions byte-by-byte, returns an integer 
 *  greater than, equal to, or less than 0, if the region pointer to by `vl` is greater 
 *  than, equal to, or less than the region pointer by `vr`, respectively. */
LAKE_HOT_FN LAKE_NONNULL(1,2)
LAKEAPI s32 LAKECALL 
lake_memcmp(
    void const *vl, 
    void const *vr, 
    usize       n);

/** Scans the first N bytes of memory for the occurence of `c`. */
LAKE_HOT_FN LAKE_NONNULL(1)
LAKEAPI void *LAKECALL 
lake_memrchr(
    void const *restrict mem, 
    s32                  c, 
    usize                n);

/** Calculates the length of a null-terminated string. */
LAKE_HOT_FN LAKE_NONNULL(1)
LAKEAPI usize LAKECALL 
lake_strlen(
    char const *str);

/** Compares up to first N characters in a string. */
LAKE_HOT_FN LAKE_NONNULL(1,2)
LAKEAPI s32 LAKECALL 
lake_strncmp(
    char const *vl, 
    char const *vr, 
    usize       n);

/** Copies over up to N bytes from string to destination string. */
LAKE_HOT_FN LAKE_NONNULL(1,2)
LAKEAPI char *LAKECALL 
lake_strncpy(
    char       *restrict dst, 
    char const *restrict str, 
    usize                n);

/** Finds the first occurence of `c` (converted to char) in a null-terminated string,
 *  except that if `c` is not found in the string, then it returns a pointer to the 
 *  null byte at the end of the string instead of NULL. */
LAKE_HOT_FN LAKE_NONNULL(1)
LAKEAPI char *LAKECALL 
lake_strchrnul(
    char const *str, 
    s32         c);

/** Finds the first occurence of `c` (converted to char) in a null-terminated string. */
LAKE_FORCE_INLINE char *lake_strchr(char const *str, s32 c)
{ char *r = lake_strchrnul(str, c); return *lake_reinterpret_cast(u8 *, r) == lake_static_cast(u8, c) ? r : 0; }

/** Finds the last occurence of `c` (converted to char) in a null-terminated string. */
LAKE_FORCE_INLINE char *lake_strrchr(char const *str, s32 c) 
{ return lake_reinterpret_cast(char *, lake_memrchr(lake_reinterpret_cast(void const *, str), c, lake_strlen(str) + 1)); }

/** Aligned OS allocator, used temporarily until i implement other strategies 
 *  for memory allocation, and how to integrate them with my fiber scheduler.
 *  They will later be replaced, and code will be refactored. TODO */
LAKE_HOT_FN 
LAKEAPI void *LAKECALL 
__lake_malloc(
    usize size, 
    usize align);

#define __lake_malloc_t(T)    __lake_malloc(sizeof(T), alignof(T))
#define __lake_malloc_n(T, n) __lake_malloc(sizeof(T) * (n), alignof(T))

/** Reallocates memory allocated from `__lake_malloc_()`. */
LAKE_HOT_FN 
LAKEAPI void *LAKECALL 
__lake_realloc(
    void *ptr, 
    usize size, 
    usize align);

#define __lake_realloc_t(ptr, T)    __lake_realloc(ptr, sizeof(T), alignof(T))
#define __lake_realloc_n(ptr, T, n) __lake_realloc(ptr, sizeof(T) * (n), alignof(T))

/** Frees memory allocated from `__lake_malloc_()`. */
LAKE_HOT_FN 
LAKEAPI void LAKECALL 
__lake_free(
    void *ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */
