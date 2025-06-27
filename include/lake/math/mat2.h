#pragma once

/** @file lake/math/mat2.h
 *  @brief TODO docs
 */
#include <lake/bedrock/simd.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(LAKE_ARCH_X86_SSE2)

LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_mat2_mul_sse2(mat2 m1, mat2 m2, mat2 dest);

LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat2_transp_sse2(mat2 m, mat2 dest) 
{
    /* d c b a */
    /* d b c a */
    lake_simd_write(dest[0], lake_simd_shuffle1(lake_simd_read(m[0]), 3, 1, 2, 0));
}
#endif /* LAKE_ARCH_X86_SSE2 */

#ifdef __cplusplus
}
#endif /* __cplusplus */
