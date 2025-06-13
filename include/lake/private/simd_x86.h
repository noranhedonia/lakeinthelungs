#ifndef LAKE_SIMD_H
#error The `x86` SIMD header must not be included outside of `lake/bedrock/simd.h`
#endif
#define LAKE_SIMD_X86 1

#include <lake/types.h>

typedef __m128  f128;
typedef __m128i s128;
#define LAKE_SIMD_HAS_f128 1
#define LAKE_SIMD_HAS_s128 1

#ifdef LAKE_ARCH_X86_AVX
typedef __m256  f256;
typedef __m256i s256;
#define LAKE_SIMD_HAS_f256 1
#define LAKE_SIMD_HAS_s256 1
#endif /* AVX */

#ifdef LAKE_SIMD_UNALIGNED
#define lake_simd_read(p)       _mm_loadu_ps(p)
#define lake_simd_write(p,a)    _mm_storeu_ps(p,a)
#else
#define lake_simd_read(p)       _mm_load_ps(p)
#define lake_simd_write(p,a)    _mm_store_ps(p,a)
#endif /* LAKE_SIMD_UNALIGNED */

#ifdef LAKE_ARCH_X86_AVX
#define lake_simd_shuffle1(xmm, z, y, x, w) \
    _mm_permute_ps((xmm), _MM_SHUFFLE(z, y, x, w))
#else
#if defined(LAKE_SIMD_USE_INT_DOMAIN) && LAKE_ARCH_X86_SSE
#define lake_simd_shuffle1(xmm, z, y, x, w) \
    _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(xmm), _MM_SHUFFLE(z, y, x, w)))
#else
#define lake_simd_shuffle1(xmm, z, y, x, w) \
    _mm_shuffle_ps(xmm, xmm, _MM_SHUFFLE(z, y, x, w))
#endif
#endif /* AVX */

#define lake_simd_splat(x, lane) lake_simd_shuffle1(x, lane, lane, lane, lane)

#ifdef LAKE_ARCH_X86_AVX
#define lake_simd_set1(x)       _mm_broadcast_ss(&x)
#define lake_simd_set1_ptr(x)   _mm_broadcast_ss(x)
#define lake_simd_set1_rval(x)  _mm_set1_ps(x)
#ifdef LAKE_ARCH_X86_AVX2
#define lake_simd_splat_x(x)    _mm_broadcastss_ps(x)
#else
#define lake_simd_splat_x(x)    _mm_permute_ps(x, _MM_SHUFFLE(0, 0, 0, 0))
#endif /* AVX2 */
#define lake_simd_splat_y(y)    _mm_permute_ps(y, _MM_SHUFFLE(1, 1, 1, 1))
#define lake_simd_splat_z(z)    _mm_permute_ps(z, _MM_SHUFFLE(2, 2, 2, 2))
#define lake_simd_splat_w(w)    _mm_permute_ps(w, _MM_SHUFFLE(3, 3, 3, 3))
#else
#define lake_simd_set1(x)       _mm_set1_ps(x)
#define lake_simd_set1_ptr(x)   _mm_set1_ps(*x)
#define lake_simd_set1_rval(x)  _mm_set1_ps(x)
#define lake_simd_splat_x(x)    lake_simd_splat(x, 0)
#define lake_simd_splat_y(y)    lake_simd_splat(y, 1)
#define lake_simd_splat_z(z)    lake_simd_splat(z, 2)
#define lake_simd_splat_w(w)    lake_simd_splat(w, 3)
#endif /* AVX */

#define lake_simd_shuffle2(a, b, z0, y0, x0, w0, z1, y1, x1, w1) \
    lake_simd_shuffle1(_mm_shuffle_ps(a, b, _MM_SHUFFLE(z0, y0, x0, w0)), z1, y1, x1, w1)

#ifdef LAKE_ARCH_X86_AVX
#ifdef LAKE_SIMD_UNALIGNED
#define lake_simd256_read(p)    _mm256_loadu_ps(p)
#define lake_simd256_write(p)   _mm256_storeu_ps(p)
#endif
#define lake_simd256_read(p)    _mm256_load_ps(p)
#define lake_simd256_write(p,a) _mm256_store_ps(p,a)
#endif /* AVX */

/* note that `0x80000000` corresponds to `INT_MIN` for a 32-bit int */
#ifdef LAKE_ARCH_X86_SSE2
#define LAKE_SIMD_NEGZEROf      ((s32)0x80000000) /* -> -0.0f */
#define LAKE_SIMD_POSZEROf      ((s32)0x00000000) /* -> +0.0f */
#else
#ifdef LAKE_SIMD_FAST_MATH
union { s32 i; f32 f; } static LAKE_SIMD_NEGZEROf_TU = { .i = (s32)0x80000000 };
#define LAKE_SIMD_NEGZEROf  LAKE_SIMD_NEGZEROf_TU.f
#define LAKE_SIMD_POSZEROf  0.0f
#else
#define LAKE_SIMD_NEGZEROf -0.0f
#define LAKE_SIMD_POSZEROf  0.0f
#endif
#endif /* SSE2 */

#ifdef LAKE_ARCH_X86_SSE2
#define LAKE_SIMD_SIGNMASKf(x,y,z,w) \
    _mm_castsi128_ps(_mm_set_epi32(x,y,z,w))
#else
#define LAKE_SIMD_SIGNMASKf(x,y,z,w) \
    _mm_set_ps(x,y,z,w)
#endif /* SSE2 */

#define lake_simd_float32x4_SIGNMASK_PNPN \
    LAKE_SIMD_SIGNMASKf(LAKE_SIMD_POSZEROf, LAKE_SIMD_NEGZEROf, LAKE_SIMD_POSZEROf, LAKE_SIMD_NEGZEROf)
#define lake_simd_float32x4_SIGNMASK_NPNP \
    LAKE_SIMD_SIGNMASKf(LAKE_SIMD_NEGZEROf, LAKE_SIMD_POSZEROf, LAKE_SIMD_NEGZEROf, LAKE_SIMD_POSZEROf)
#define lake_simd_float32x4_SIGNMASK_NPPN \
    LAKE_SIMD_SIGNMASKf(LAKE_SIMD_NEGZEROf, LAKE_SIMD_POSZEROf, LAKE_SIMD_POSZEROf, LAKE_SIMD_NEGZEROf)

/* fast math prevents -0.0f to work */
#if defined(LAKE_ARCH_X86_SSE2)
#define lake_simd_float32x4_SIGNMASK_NEG _mm_castsi128_ps(_mm_set1_epi32(LAKE_SIMD_NEGZEROf))
#else
#define lake_simd_float32x4_SIGNMASK_NEG _mm_set1_ps(LAKE_SIMD_NEGZEROf)
#endif
#define lake_simd_float32x8_SIGNMASK_NEG _mm256_castsi256_ps(_mm256_set1_epi32(LAKE_SIMD_NEGZEROf))

LAKE_FORCE_INLINE f128 lake_simd_abs(f128 x) 
{ return _mm_andnot_ps(lake_simd_float32x4_SIGNMASK_NEG, x); }

LAKE_FORCE_INLINE f128 lake_simd_min(f128 a, f128 b) 
{ return _mm_min_ps(a, b); }

LAKE_FORCE_INLINE f128 lake_simd_max(f128 a, f128 b) 
{ return _mm_max_ps(a, b); }

LAKE_FORCE_INLINE f128 lake_simd_vhadd(f128 v) 
{
    f128 x0;
    x0 = _mm_add_ps(v, lake_simd_shuffle1(v, 0, 1, 2, 3));
    x0 = _mm_add_ps(x0, lake_simd_shuffle1(x0, 1, 0, 0, 1));
    return x0;
}

LAKE_FORCE_INLINE f128 lake_simd_vhadds(f128 v)
{ 
    f128 shuf, sums;
#if LAKE_ARCH_X86_SSE3
    shuf = _mm_movehdup_ps(v);
#else
    shuf = lake_simd_shuffle1(v, 2, 3, 0, 1);
#endif /* SSE3 */
    sums = _mm_add_ps(v, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ss(sums, shuf);
    return sums;
}

LAKE_FORCE_INLINE f32 lake_simd_hadd(f128 v)
{ return _mm_cvtss_f32(lake_simd_vhadds(v)); }

LAKE_FORCE_INLINE f128 lake_simd_vhmin(f128 v)
{
    f128 x0, x1, x2;
    x0 = _mm_movehl_ps(v, v);    /* [ 2,  3,  2,  3 ] */
    x1 = _mm_min_ps(x0, v);      /* [0|2 1|3 2|3 3|3] */
    x2 = lake_simd_splat(x1, 1); /* [1|3 1|3 1|3 1|3] */
    return _mm_min_ss(x1, x2);
}

LAKE_FORCE_INLINE f32 lake_simd_hmin(f128 v)
{ return _mm_cvtss_f32(lake_simd_vhmin(v)); }

LAKE_FORCE_INLINE f128 lake_simd_vhmax(f128 v)
{
    f128 x0, x1, x2;
    x0 = _mm_movehl_ps(v, v);    /* [ 2,  3,  2,  3 ] */
    x1 = _mm_max_ps(x0, v);      /* [0|2 1|3 2|3 3|3] */
    x2 = lake_simd_splat(x1, 1); /* [1|3 1|3 1|3 1|3] */
    return _mm_max_ss(x1, x2);
}

LAKE_FORCE_INLINE f32 lake_simd_hmax(f128 v)
{ return _mm_cvtss_f32(lake_simd_vhmax(v)); }

LAKE_FORCE_INLINE f128 lake_simd_vdots(f128 a, f128 b)
{
#if defined(LAKE_ARCH_X86_SSE4_1) || defined(LAKE_ARCH_X86_SSE4_2)
    return _mm_dp_ps(a, b, 0xFF);
#elif defined(LAKE_ARCH_X86_SSE3)
    f128 x0, x1;
    x0 = _mm_mul_ps(a, b);
    x1 = _mm_hadd_ps(x0, x0);
    return _mm_hadd_ps(x1, x1);
#else
    return lake_simd_vhadds(_mm_mul_ps(a, b));
#endif /* SSE4_1 SSE4_2 */
}

LAKE_FORCE_INLINE f128 lake_simd_vdot(f128 a, f128 b)
{
#if defined(LAKE_ARCH_X86_SSE4_1) || defined(LAKE_ARCH_X86_SSE4_2)
    return _mm_dp_ps(a, b, 0xFF);
#elif defined(LAKE_ARCH_X86_SSE3)
    f128 x0, x1;
    x0 = _mm_mul_ps(a, b);
    x1 = _mm_hadd_ps(x0, x0);
    return _mm_hadd_ps(x1, x1);
#else
    f128 x0;
    x0 = _mm_mul_ps(a, b);
    x0 = _mm_add_ps(x0, lake_simd_shuffle1(x0, 1, 0, 3, 2));
    return _mm_add_ps(x0, lake_simd_shuffle1(x0, 0, 1, 0, 1));
#endif /* SSE4_1 SSE4_2 */
}

LAKE_FORCE_INLINE f32 lake_simd_dot(f128 a, f128 b) 
{ return _mm_cvtss_f32(lake_simd_vdots(a, b)); }

LAKE_FORCE_INLINE f32 lake_simd_norm(f128 a)
{ return _mm_cvtss_f32(_mm_sqrt_ss(lake_simd_vhadds(_mm_mul_ps(a, a)))); }

LAKE_FORCE_INLINE f32 lake_simd_norm2(f128 a)
{ return _mm_cvtss_f32(lake_simd_vhadds(_mm_mul_ps(a, a))); }

LAKE_FORCE_INLINE f32 lake_simd_norm_one(f128 a)
{ return _mm_cvtss_f32(lake_simd_vhadds(lake_simd_abs(a))); }

LAKE_FORCE_INLINE f32 lake_simd_norm_inf(f128 a)
{ return _mm_cvtss_f32(lake_simd_vhmax(lake_simd_abs(a))); }

#if defined(LAKE_ARCH_X86_SSE2)
LAKE_FORCE_INLINE f128 lake_simd_read3f(vec3 v)
{
    s128 xy;
    f128 z;
    xy = _mm_loadl_epi64(lake_assume_aligned_cast(const s128, v));
    z = _mm_load_ss(&v[2]);
    return _mm_movelh_ps(_mm_castsi128_ps(xy), z);
}

LAKE_FORCE_INLINE void lake_simd_write3f(vec3 v, f128 vx)
{
    _mm_storel_pi(lake_assume_aligned_cast(__m64, v), vx);
    _mm_store_ss(&v[2], lake_simd_shuffle1(vx, 2, 2, 2, 2));
}
#endif /* SSE2 */

LAKE_FORCE_INLINE f128 lake_simd_div(f128 a, f128 b)
{ return _mm_div_ps(a, b); }

LAKE_FORCE_INLINE f128 lake_simd_fmadd(f128 a, f128 b, f128 c)
{
#ifdef LAKE_ARCH_X86_FMA
    return _mm_fmadd_ps(a, b, c);
#else
    return _mm_add_ps(c, _mm_mul_ps(a, b));
#endif
}

LAKE_FORCE_INLINE f128 lake_simd_fnmadd(f128 a, f128 b, f128 c)
{
#ifdef LAKE_ARCH_X86_FMA
    return _mm_fnmadd_ps(a, b, c);
#else
    return _mm_sub_ps(c, _mm_mul_ps(a, b));
#endif
}

LAKE_FORCE_INLINE f128 lake_simd_fmsub(f128 a, f128 b, f128 c)
{
#ifdef LAKE_ARCH_X86_FMA
    return _mm_fmsub_ps(a, b, c);
#else
    return _mm_sub_ps(_mm_mul_ps(a, b), c);
#endif
}

LAKE_FORCE_INLINE f128 lake_simd_fnmsub(f128 a, f128 b, f128 c)
{
#ifdef LAKE_ARCH_X86_FMA
    return _mm_fnmsub_ps(a, b, c);
#else
    return _mm_xor_ps(_mm_add_ps(_mm_mul_ps(a, b), c), lake_simd_float32x4_SIGNMASK_NEG);
#endif
}

#ifdef LAKE_ARCH_X86_AVX
LAKE_FORCE_INLINE f256 lake_simd256_fmadd(f256 a, f256 b, f256 c)
{
#ifdef LAKE_ARCH_X86_FMA
    return _mm256_fmadd_ps(a, b, c);
#else
    return _mm256_add_ps(c, _mm256_mul_ps(a, b));
#endif
}

LAKE_FORCE_INLINE f256 lake_simd256_fnmadd(f256 a, f256 b, f256 c)
{
#ifdef LAKE_ARCH_X86_FMA
    return _mm256_fnmadd_ps(a, b, c);
#else
    return _mm256_sub_ps(c, _mm256_mul_ps(a, b));
#endif
}

LAKE_FORCE_INLINE f256 lake_simd256_fmsub(f256 a, f256 b, f256 c)
{
#ifdef LAKE_ARCH_X86_FMA
    return _mm256_fmsub_ps(a, b, c);
#else
    return _mm256_sub_ps(_mm256_mul_ps(a, b), c);
#endif
}

LAKE_FORCE_INLINE f256 lake_simd256_fnmsub(f256 a, f256 b, f256 c)
{
#ifdef LAKE_ARCH_X86_FMA
    return _mm256_fnmsub_ps(a, b, c);
#else
    return _mm256_xor_ps(_mm256_add_ps(_mm256_mul_ps(a, b), c), lake_simd_float32x8_SIGNMASK_NEG);
#endif
}
#endif /* AVX */
