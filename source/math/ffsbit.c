#include <lake/math/bits.h>

static const u8 g_ffs_table[256] = {
    0,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    7,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    8,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    7,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1
};

u32 lake_ffsbit_u32(u32 v)
{
    if (!v) return 0;

    for (u32 o = 0; o < sizeof(u32); o++) {
        u32 pos = o << 3;
        u8 byte = (v >> pos) & 0xff;
        if (byte != 0) return pos + g_ffs_table[byte];
    }
    LAKE_UNREACHABLE;
}

u64 lake_ffsbit_table_lookup(u8 const *data, usize n)
{
    for (u64 o = 0; o < n; o++)
        if (data[o] != 0) return (o << 3) + g_ffs_table[data[o]];
    return 0;
}

#if defined(LAKE_ARCH_X86_AVX2)
u64 lake_ffsbit_avx2(u8 const *data, usize n)
{
    u64 o = 0;
    while (o + 32 <= n) {
        __m256i vec  = _mm256_loadu_si256((__m256i const *)(data + o));

        /* sets the FZ flag on non-zero values, we AND vec with itself to check if any bits are set to 1 */
        if (!_mm256_testz_si256(vec, vec)) {
            __m256i cmp = _mm256_cmpeq_epi8(vec, _mm256_setzero_si256());
            __m256i nonzero = _mm256_andnot_si256(cmp, _mm256_set1_epi8((u8)0xFF));

            s32 mask = _mm256_movemask_epi8(nonzero);
            s32 idx = lake_ctz(mask);
            return ((idx + o) << 3) + g_ffs_table[data[idx + o]];
        }
        o += 32;
    }
    for (; o < n; o++)
        if (data[o] != 0) return (o << 3) + g_ffs_table[data[o]];
    return 0lu;
}
#endif /* LAKE_ARCH_X86_AVX2 */

#if defined(LAKE_ARCH_X86_SSE2)
u64 lake_ffsbit_sse2(u8 const *data, usize n)
{
    u64 o = 0;
    while (o + 16 <= n) {
        __m128i vec = _mm_loadu_si128((__m128i const *)(data + o));
#ifdef LAKE_ARCH_X86_SSE4_1
        if (!_mm_testz_si128(vec, vec)) {
            __m128i cmp = _mm_cmpeq_epi8(vec, _mm_setzero_si128());
            __m128i nonzero = _mm_andnot_si128(cmp, _mm_set1_epi8((u8)0xFF));

            s32 mask = _mm_movemask_epi8(nonzero);
            s32 idx = lake_ctz(mask);
            return ((idx + o) << 3) + g_ffs_table[data[idx + o]];
        }
#else
        __m128i cmp = _mm_cmpeq_epi8(vec, _mm_setzero_si128());
        s32 mask = _mm_movemask_epi8(cmp);
        if (mask != 0xFFFF) {
            s32 nonzero = (~mask) & 0xFFFF;
            s32 idx = lake_ctz(nonzero);
            return ((idx + o) << 3) + g_ffs_table[data[idx + o]];
        }
#endif /* LAKE_ARCH_X86_SSE4_1 */
        o += 16;
    }
    for (; o < n; o++)
        if (data[o] != 0) return (o << 3) + g_ffs_table[data[o]];
    return 0lu;
}
#endif /* LAKE_ARCH_X86_SSE2 */
