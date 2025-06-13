#include <lake/math/bits.h>

static const u8 g_popcnt_table[256] = {
	0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};

u32 lake_popcnt_u32(u32 v)
{
    if (!v) return 0;

    u32 cnt = 0;
    for (u32 o = 0; o < sizeof(u32); o++) {
        u32 pos = o << 3;
        u8 byte = (v >> pos) & 0xff;
        if (byte != 0) cnt += g_popcnt_table[byte];
    }
    return cnt;
}

u64 lake_popcnt_table_lookup(u8 const *data, usize n)
{
    u64 bits = 0;
    usize o = 0;
    while (o + 4 <= n) {
        bits += g_popcnt_table[data[o]]; o++;
        bits += g_popcnt_table[data[o]]; o++;
        bits += g_popcnt_table[data[o]]; o++;
        bits += g_popcnt_table[data[o]]; o++;
    }
    while (o < n) {
        bits += g_popcnt_table[data[o]]; o++;
    }
    return bits;
}

#if defined(LAKE_ARCH_X86_AVX2)
u64 lake_popcnt_avx2(u8 const *data, usize n)
{
    usize o = 0;
    __m256i const lookup = _mm256_setr_epi8(
        0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
        0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4);
    __m256i const low_mask = _mm256_set1_epi8((u8)0x0F);
    __m256i acc = _mm256_setzero_si256();

#define ITER {                                                                      \
        __m256i const vec = _mm256_loadu_si256((__m256i const *)(data + o));        \
        __m256i const lo  = _mm256_and_si256(vec, low_mask);                        \
        __m256i const hi  = _mm256_and_si256(_mm256_srli_epi16(vec, 4), low_mask);  \
        __m256i const popcnt1 = _mm256_shuffle_epi8(lookup, lo);                    \
        __m256i const popcnt2 = _mm256_shuffle_epi8(lookup, hi);                    \
        local = _mm256_add_epi8(local, popcnt1);                                    \
        local = _mm256_add_epi8(local, popcnt2);                                    \
        o += 32; \
    }
    while (o + 8*32 <= n) {
        __m256i local = _mm256_setzero_si256();
        LAKE_MAGIC_EVAL8(ITER)
        acc = _mm256_add_epi64(acc, _mm256_sad_epu8(local, _mm256_setzero_si256()));
    }
    __m256i local = _mm256_setzero_si256();

    while (o + 32 <= n) { ITER }
    acc = _mm256_add_epi64(acc, _mm256_sad_epu8(local, _mm256_setzero_si256()));
#undef ITER

    u64 result = 0;
    result += (u64)_mm256_extract_epi64(acc, 0);
    result += (u64)_mm256_extract_epi64(acc, 1);
    result += (u64)_mm256_extract_epi64(acc, 2);
    result += (u64)_mm256_extract_epi64(acc, 3);

    for (; o < n; o++)
        result += g_popcnt_table[data[o]];
    return result;
}
#endif /* LAKE_ARCH_X86_AVX2 */

#if defined(LAKE_ARCH_X86_SSE2)
#if defined(LAKE_ARCH_X86_SSSE3)
    #define SHUFFLE_EPI8(v, mask) _mm_shuffle_epi8(v, mask)
#else
LAKE_FORCE_INLINE __m128i COMGE_EPI8(__m128i a, __m128i b) 
{
    __m128i c;
    c = _mm_cmpgt_epi8(a, b);
    a = _mm_cmpeq_epi8(a, b);
    a = _mm_or_si128(a, c);
    return a;
}
LAKE_FORCE_INLINE __m128i SHUFFLE_EPI8(__m128i a, __m128i mask) 
{
    union { __m128i i; s8 s[16]; } A, B, MASK, MASKZERO; \
    A.i = a; \
    MASKZERO.i = COMGE_EPI8(mask, _mm_setzero_si128()); \
    MASK.i = _mm_and_si128(mask, _mm_set1_epi8((char)0x0f)); \
    \
    B.s[0] = A.s[(MASK.s[0])]; \
    B.s[1] = A.s[(MASK.s[1])]; \
    B.s[2] = A.s[(MASK.s[2])]; \
    B.s[3] = A.s[(MASK.s[3])]; \
    B.s[4] = A.s[(MASK.s[4])]; \
    B.s[5] = A.s[(MASK.s[5])]; \
    B.s[6] = A.s[(MASK.s[6])]; \
    B.s[7] = A.s[(MASK.s[7])]; \
    B.s[8] = A.s[(MASK.s[8])]; \
    B.s[9] = A.s[(MASK.s[9])]; \
    B.s[10] = A.s[(MASK.s[10])]; \
    B.s[11] = A.s[(MASK.s[11])]; \
    B.s[12] = A.s[(MASK.s[12])]; \
    B.s[13] = A.s[(MASK.s[13])]; \
    B.s[14] = A.s[(MASK.s[14])]; \
    B.s[15] = A.s[(MASK.s[15])]; \
    B.i = _mm_and_si128(B.i, MASKZERO.i);
    return B.i;
}
#endif /* LAKE_ARCH_X86_SSSE3 */

u64 lake_popcnt_sse2(u8 const *data, usize n)
{
    usize o = 0;
    __m128i const lookup = _mm_setr_epi8(
        0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4);
    __m128i const low_mask = _mm_set1_epi8((u8)0x0F);
    __m128i acc = _mm_setzero_si128();

#define ITER {                                                                  \
        __m128i const vec = _mm_loadu_si128((__m128i const *)(data + o));       \
        __m128i const lo  = _mm_and_si128(vec, low_mask);                       \
        __m128i const hi  = _mm_and_si128(_mm_srli_epi16(vec, 4), low_mask);    \
        __m128i const popcnt1 = SHUFFLE_EPI8(lookup, lo);                       \
        __m128i const popcnt2 = SHUFFLE_EPI8(lookup, hi);                       \
        local = _mm_add_epi8(local, popcnt1);                                   \
        local = _mm_add_epi8(local, popcnt2);                                   \
        o += 16;                                                                \
    }
    while (o + 8*16 <= n) {
        __m128i local = _mm_setzero_si128();
        LAKE_MAGIC_EVAL8(ITER)
        acc = _mm_add_epi64(acc, _mm_sad_epu8(local, _mm_setzero_si128()));
    }
    __m128i local = _mm_setzero_si128();

    while (o + 16 <= n) { ITER }
    acc = _mm_add_epi64(acc, _mm_sad_epu8(local, _mm_setzero_si128()));
#undef ITER
#undef SHUFFLE_EPI8

    u64 result = _mm_cvtsi128_si64(acc) + _mm_cvtsi128_si64(_mm_srli_si128(acc, 8));

    for (; o < n; o++)
        result += g_popcnt_table[data[o]];
    return result;
}
#endif /* LAKE_ARCH_X86_SSE2 */
