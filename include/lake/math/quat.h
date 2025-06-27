#pragma once

/** @file lake/math/quat.h
 *  @brief TODO docs
 */
#include <lake/bedrock/simd.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(LAKE_ARCH_X86_SSE2)

LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_quat_mul_sse2(quat p, quat q, quat dest);

#endif /* LAKE_ARCH_X86_SSE2 */

/** Convert mat4's rotation part to quaternion. */
LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_quat_from_mat4(mat4 m, quat dest);

#ifdef __cplusplus
}
#endif /* __cplusplus */
