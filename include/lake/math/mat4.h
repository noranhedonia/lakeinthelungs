#pragma once

/** @file lake/math/mat3.h
 *  @brief TODO docs
 */
#include <lake/bedrock/simd.h>
#include <lake/bedrock/log.h>
#include <lake/math/vec4.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Initializes a mat4 as an identity matrix. */
#define LAKE_MAT4_IDENTITY_INIT {{1.0f, 0.0f, 0.0f, 0.0f}, \
                                {0.0f, 1.0f, 0.0f, 0.0f}, \
                                {0.0f, 0.0f, 1.0f, 0.0f}, \
                                {0.0f, 0.0f, 0.0f, 1.0f}}

/** Initializes a mat4 with zeroes. */
#define LAKE_MAT4_ZERO_INIT     {{0.0f, 0.0f, 0.0f, 0.0f}, \
                                {0.0f, 0.0f, 0.0f, 0.0f}, \
                                {0.0f, 0.0f, 0.0f, 0.0f}, \
                                {0.0f, 0.0f, 0.0f, 0.0f}}

#define LAKE_MAT4_IDENTITY      ((mat4)LAKE_MAT4_IDENTITY_INIT)
#define LAKE_MAT4_ZERO          ((mat4)LAKE_MAT4_ZERO_INIT)

/** Copy all members of mat to dest. The matrix may not be aligned,
 *  u stands for unaligned. This may be useful when copying a matrix 
 *  from an external source (from importing assets, for example). */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_ucopy(mat4 m, mat4 dest) 
{
    dest[0][0] = m[0][0];  dest[1][0] = m[1][0];
    dest[0][1] = m[0][1];  dest[1][1] = m[1][1];
    dest[0][2] = m[0][2];  dest[1][2] = m[1][2];
    dest[0][3] = m[0][3];  dest[1][3] = m[1][3];

    dest[2][0] = m[2][0];  dest[3][0] = m[3][0];
    dest[2][1] = m[2][1];  dest[3][1] = m[3][1];
    dest[2][2] = m[2][2];  dest[3][2] = m[3][2];
    dest[2][3] = m[2][3];  dest[3][3] = m[3][3];
}

/** Copy all members of mat to dest. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_copy(mat4 m, mat4 dest) 
{
#if defined(LAKE_ARCH_X86_AVX)
  lake_simd256_write(dest[0], lake_simd256_read(m[0]));
  lake_simd256_write(dest[2], lake_simd256_read(m[2]));
#elif defined(LAKE_ARCH_X86_SSE2)
  lake_simd_write(dest[0], lake_simd_read(m[0]));
  lake_simd_write(dest[1], lake_simd_read(m[1]));
  lake_simd_write(dest[2], lake_simd_read(m[2]));
  lake_simd_write(dest[3], lake_simd_read(m[3]));
#else
  lake_mat4_ucopy(m, dest);
#endif
}

/** Make given matrix an identity. It is easier to use it for member matrices. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_identity(mat4 m) 
{
    LAKE_SIMD_ALIGNMENT mat4 temp = LAKE_MAT4_IDENTITY_INIT;
    lake_mat4_copy(temp, m);
}

/** Make given matrix array's each element identity matrix. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_identity_array(mat4 *restrict m, u32 count)
{
    LAKE_SIMD_ALIGNMENT mat4 temp = LAKE_MAT4_IDENTITY_INIT;
    for (usize i = 0; i < count; lake_mat4_copy(temp, m[i++]));
}

/** Make a given matrix zero. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_zero(mat4 m) 
{
#if defined(LAKE_ARCH_X86_AVX)
    f256 x0;
    x0 = _mm256_setzero_ps();
    lake_simd256_write(m[0], x0);
    lake_simd256_write(m[2], x0);
#elif defined(LAKE_ARCH_X86_SSE2)
    f128 x0;
    x0 = _mm_setzero_ps();
    lake_simd_write(m[0], x0);
    lake_simd_write(m[1], x0);
    lake_simd_write(m[2], x0);
    lake_simd_write(m[3], x0);
#else
    LAKE_SIMD_ALIGNMENT mat4 temp = LAKE_MAT4_ZERO_INIT;
    lake_mat4_copy(temp, m);
#endif
}

/** Copy upper-left of mat4 to mat3. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_pick3(mat4 m, mat3 dest)
{
    dest[0][0] = m[0][0];
    dest[0][1] = m[0][1];
    dest[0][2] = m[0][2];

    dest[1][0] = m[1][0];
    dest[1][1] = m[1][1];
    dest[1][2] = m[1][2];

    dest[2][0] = m[2][0];
    dest[2][1] = m[2][1];
    dest[2][2] = m[2][2];
}

/** Copy upper-left of mat4 to mat3 (transposed). */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_pick3t(mat4 m, mat3 dest)
{
    dest[0][0] = m[0][0];
    dest[0][1] = m[1][0];
    dest[0][2] = m[2][0];

    dest[1][0] = m[0][1];
    dest[1][1] = m[1][1];
    dest[1][2] = m[2][1];

    dest[2][0] = m[0][2];
    dest[2][1] = m[1][2];
    dest[2][2] = m[2][2];
}

#if defined(LAKE_ARCH_X86_AVX)

LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_mat4_scale_avx(mat4 m, f32 s);

LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_mat4_mul_avx(mat4 m1, mat4 m2, mat4 dest);

#endif /* LAKE_ARCH_X86_AVX */

#if defined(LAKE_ARCH_X86_SSE2)

LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_mat4_scale_sse2(mat4 m, float s);

LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_mat4_transp_sse2(mat4 m, mat4 dest);

LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_mat4_mul_sse2(mat4 m1, mat4 m2, mat4 dest);

LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_mat4_mulv_sse2(mat4 m, vec4 v, vec4 dest);

LAKEAPI LAKE_NONNULL_ALL
f32 LAKECALL lake_mat4_det_sse2(mat4 m);

LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_mat4_inv_fast_sse2(mat4 m, mat4 dest);

LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_mat4_inv_sse2(mat4 m, mat4 dest);

#define lake_mat4_inv_precise_sse2(mat, dest) \
    lake_mat4_inv_sse2(mat, dest)
#endif /* LAKE_ARCH_X86_SSE2 */

/** Scale (multiply with scalar) a matrix without simd optimization. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_scale_p(mat4 m, f32 s) 
{
    m[0][0] *= s; m[0][1] *= s; m[0][2] *= s; m[0][3] *= s;
    m[1][0] *= s; m[1][1] *= s; m[1][2] *= s; m[1][3] *= s;
    m[2][0] *= s; m[2][1] *= s; m[2][2] *= s; m[2][3] *= s;
    m[3][0] *= s; m[3][1] *= s; m[3][2] *= s; m[3][3] *= s;
}

/** Scale (multiply with scalar) a matrix. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_scale(mat4 m, f32 s) 
{
#if defined(LAKE_ARCH_X86_AVX)
    lake_mat4_scale_avx(m, s);
#elif defined(LAKE_ARCH_X86_SSE2)
    lake_mat4_scale_sse2(m, s);
#else
    lake_mat4_scale_p(m, s);
#endif
}

LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_mat4_mul_p(mat4 m1, mat4 m2, mat4 dest);

/** Multiply m1 and m2 to dest -- m1, m2 and dest can be the same matrix. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_mul(mat4 m1, mat4 m2, mat4 dest) 
{
#if defined(LAKE_ARCH_X86_AVX)
    lake_mat4_mul_avx(m1, m2, dest);
#elif defined(LAKE_ARCH_X86_SSE2)
    lake_mat4_mul_sse2(m1, m2, dest);
#else
    lake_mat4_mul_p(m1, m2, dest);
#endif
}

/** Multiply N mat4 matrices and store result in dest. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_muln(mat4 *restrict matrices[], u32 n, mat4 dest) 
{
    LAKE_ASSUME(n > 1 && "there must be atleast 2 matrices");
    lake_mat4_mul(*matrices[0], *matrices[1], dest);
    for (u32 i = 2; i < n; lake_mat4_mul(dest, *matrices[i++], dest));
}

/** Multiply mat4 with vec4 (column vector) and store in dest vector. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_mulv(mat4 m, vec4 v, vec4 dest)
{
#if defined(LAKE_ARCH_X86_SSE2)
    lake_mat4_mulv_sse2(m, v, dest);
#else
    vec4 res;
    res[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
    res[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
    res[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
    res[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
    lake_vec4_copy(res, dest);
#endif
}

/** Trace of matrix. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE f32 lake_mat4_trace(mat4 m)
{ return m[0][0] + m[1][1] + m[2][2] + m[3][3]; }

/** Trace of matrix (rotation part). */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE f32 lake_mat4_trace3(mat4 m)
{ return m[0][0] + m[1][1] + m[2][2]; }

#ifdef __cplusplus
}
#endif /* __cplusplus */
