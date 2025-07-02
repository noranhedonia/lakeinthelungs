#pragma once

/** @file lake/math/crypto.h
 *  @brief Cryptography.
 *
 *  There is not much, I just need SHA-256, maybe other stuff too.
 */
#include <lake/bedrock/simd.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* SHA-256 */
typedef u8 LAKE_SIMD_ALIGNMENT lake_oid[32];

LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_crypto_sha256_p(void const *data, usize size, lake_oid dest);

#ifdef LAKE_ARCH_X86_AVX2
LAKEAPI LAKE_NONNULL_ALL
void LAKECALL lake_crypto_sha256_avx2(void const *data, usize size, lake_oid dest);
#endif /* LAKE_ARCH_X86_AVX2 */

LAKE_FORCE_INLINE LAKE_NONNULL_ALL
void lake_crypto_sha256(void const *data, usize size, lake_oid dest)
{
#ifdef LAKE_ARCH_X86_AVX2
    lake_crypto_sha256_avx2(data, size, dest);
#else
    lake_crypto_sha256_p(data, size, dest);
#endif
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
