#pragma once

/** @file lake/math/vector.h
 *  @brief TODO docs
 */
#include <lake/bedrock/simd.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Initializes a vec4 with ones. */
#define LAKE_VEC4_ONE_INIT   {1.0f, 1.0f, 1.0f, 1.0f}
/** Initializes a vec4 with the black color. */
#define LAKE_VEC4_BLACK_INIT {0.0f, 0.0f, 0.0f, 1.0f}
/** Initializes a vec4 with zeroes. */
#define LAKE_VEC4_ZERO_INIT  {0.0f, 0.0f, 0.0f, 0.0f}

#define LAKE_VEC4_ONE   ((vec4)LAKE_VEC4_ONE_INIT)
#define LAKE_VEC4_BLACK ((vec4)LAKE_VEC4_BLACK_INIT)
#define LAKE_VEC4_ZERO  ((vec4)LAKE_VEC4_ZERO_INIT)

#define LAKE_VEC4_XXXX  lake_shuffle4(0, 0, 0, 0)   
#define LAKE_VEC4_YYYY  lake_shuffle4(1, 1, 1, 1)   
#define LAKE_VEC4_ZZZZ  lake_shuffle4(2, 2, 2, 2)   
#define LAKE_VEC4_WWWW  lake_shuffle4(3, 3, 3, 3)   
#define LAKE_VEC4_WZYX  lake_shuffle4(0, 1, 2, 3)   

/** Init vec4 using vec3. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_vec4(vec3 v3, f32 last, vec4 dest)
{
    dest[0] = v3[0];
    dest[1] = v3[1];
    dest[2] = v3[2];
    dest[3] = last;
}

/** Copy first 3 members of a vec3 to dest vec3. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_vec4_copy3(vec4 v, vec3 dest)
{
    dest[0] = v[0];
    dest[1] = v[1];
    dest[2] = v[2];
}

/** Copy all members of a vec4 to dest vec4, unaligned. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_vec4_ucopy(vec4 v, vec4 dest)
{
    dest[0] = v[0];
    dest[1] = v[1];
    dest[2] = v[2];
    dest[3] = v[3];
}

/** Copy all members of a vec4 to dest vec4. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_vec4_copy(vec4 v, vec4 dest)
{
#if LAKE_SIMD
    lake_simd_write(dest, lake_simd_read(v));
#else
    lake_vec4_ucopy(v, dest);
#endif
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
