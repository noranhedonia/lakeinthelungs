#pragma once

/** @file lake/math/bits.h
 *
 *  The article on fast population-count by Wojciech Muła, and his github repository
 *  were very helpful in implementing these operations:
 *
 *  https://github.com/WojciechMula/sse-popcount/tree/master,
 *  http://0x80.pl/notesen/2008-05-24-sse-popcount.html 
 *
 *  The paper listed below, by Wojciech Muła, Nathan Kurz and Daniel Lemire was a great 
 *  deal in understanding the different algorithms that could be implemented here:
 *
 *  https://www.google.com/url?q=https://arxiv.org/pdf/1611.07612&sa=U&ved=2ahUKEwiz2pCwxZaLAxVpwjgGHSvfNpoQFnoECAoQAg&usg=AOvVaw3WMSV89LThBSsU2_fZApCR 
 */
#include <lake/bedrock/simd.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

LAKEAPI LAKE_HOT_FN LAKE_CONST_FN 
u32 LAKECALL lake_ffsbit_u32(u32 v);

LAKEAPI LAKE_HOT_FN LAKE_CONST_FN 
u32 LAKECALL lake_popcnt_u32(u32 v);

/** Find first set bit default implementation via a static lookup table. */
LAKE_NONNULL(1) LAKE_PURE_FN
LAKEAPI u64 LAKECALL lake_ffsbit_table_lookup(u8 const *data, usize n);

/** Population count default implementation via a static lookup table. */
LAKE_NONNULL(1) LAKE_PURE_FN
LAKEAPI u64 LAKECALL lake_popcnt_table_lookup(u8 const *data, usize n);

#if defined(LAKE_ARCH_X86_AVX2)
LAKE_NONNULL(1) LAKE_PURE_FN
LAKEAPI u64 LAKECALL lake_ffsbit_avx2(u8 const *data, usize n);

LAKE_NONNULL(1) LAKE_PURE_FN
LAKEAPI u64 LAKECALL lake_popcnt_avx2(u8 const *data, usize n);
#endif /* LAKE_ARCH_X86_AVX2 */

#if defined(LAKE_ARCH_X86_SSE2)
LAKE_NONNULL(1) LAKE_PURE_FN
LAKEAPI u64 LAKECALL lake_ffsbit_sse2(u8 const *data, usize n);

LAKE_NONNULL(1) LAKE_PURE_FN
LAKEAPI u64 LAKECALL lake_popcnt_sse2(u8 const *data, usize n);
#endif /* LAKE_ARCH_X86_SSE2 */

/* TODO neon, rvv, wasm */

/** Find first set bit in an array of bytes. Returns a 1-based value that 
 *  indicates a bit position, or 0 if all bits were zeroes. */
LAKE_FORCE_INLINE LAKE_NONNULL(1) LAKE_PURE_FN
u64 lake_ffsbit(u8 const *data, usize n)
{
#if defined(LAKE_ARCH_X86_AVX2)
    return n >= 32 ? lake_ffsbit_avx2(data, n) : lake_ffsbit_sse2(data, n);
#elif defined(LAKE_ARCH_X86_SSE2)
    return lake_ffsbit_sse2(data, n);
#else
    return lake_ffsbit_table_lookup(data, n);
#endif
}

/** Count set bits in an array of bytes. */
LAKE_FORCE_INLINE LAKE_NONNULL(1) LAKE_PURE_FN
u64 lake_popcnt(u8 const *data, usize n)
{
#if defined(LAKE_ARCH_X86_AVX2)
    return n >= 32 ? lake_popcnt_avx2(data, n) : lake_popcnt_sse2(data, n);
#elif defined(LAKE_ARCH_X86_SSE2)
    return lake_popcnt_sse2(data, n);
#else
    return lake_popcnt_table_lookup(data, n);
#endif
}

/** Count trailing zeroes. */
LAKE_FORCE_INLINE LAKE_PURE_FN
s32 lake_ctz(u32 x)
{
#if LAKE_HAS_BUILTIN(__builtin_ctz)
    return __builtin_ctz(x);
#elif defined(LAKE_CC_MSVC_VERSION)
    u32 index;
    return _BitScanForward(&index, x) ? index : 32;
#else
    if (x == 0) 
        return 32;
    u32 count = 0;
    while ((x & 1) == 0) {
        count++;
        x >>= 1;
    }
    return count;
#endif
}

/** Computes the bit of the next power of 2. */
LAKE_FORCE_INLINE LAKE_CONST_FN
u32 lake_bits_next_pow2(u32 n) 
{
    if (lake_is_pow2(n)) {
        n <<= 1;
    } else {
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        n += 1;
    }
    return n;
}

/** Computes a log2 from a power of 2 value. */ 
LAKE_FORCE_INLINE LAKE_CONST_FN
u32 lake_bits_next_pow2_log2(u32 n) 
{
    if (n == 0) return 1;
    /* subtract 1 as ffsbit returns a 1-based value */
    return lake_ffsbit_u32(lake_bits_next_pow2(n)) - 1;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
