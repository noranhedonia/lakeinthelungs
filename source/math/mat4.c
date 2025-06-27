#include <lake/math/mat4.h>

#if defined(LAKE_ARCH_X86_AVX)
void lake_mat4_scale_avx(mat4 m, f32 s)
{
    f256 y0;
    y0 = _mm256_set1_ps(s);
    lake_simd256_write(m[0], _mm256_mul_ps(y0, lake_simd256_read(m[0])));
    lake_simd256_write(m[2], _mm256_mul_ps(y0, lake_simd256_read(m[2])));
}

void lake_mat4_mul_avx(mat4 m1, mat4 m2, mat4 dest)
{
    /* D = R * L (column-major) */
    f256 y0, y1, y2, y3, y4, y5, y6, y7, y8, y9;

    y0 = lake_simd256_read(m2[0]); /* h g f e d c b a */
    y1 = lake_simd256_read(m2[2]); /* p o n m l k j i */

    y2 = lake_simd256_read(m1[0]); /* h g f e d c b a */
    y3 = lake_simd256_read(m1[2]); /* p o n m l k j i */

    /* 0x03: 0b00000011 */
    y4 = _mm256_permute2f128_ps(y2, y2, 0x03); /* d c b a h g f e */
    y5 = _mm256_permute2f128_ps(y3, y3, 0x03); /* l k j i p o n m */

    /* f f f f a a a a */
    /* h h h h c c c c */
    /* e e e e b b b b */
    /* g g g g d d d d */
    y6 = _mm256_permutevar_ps(y0, _mm256_set_epi32(1, 1, 1, 1, 0, 0, 0, 0));
    y7 = _mm256_permutevar_ps(y0, _mm256_set_epi32(3, 3, 3, 3, 2, 2, 2, 2));
    y8 = _mm256_permutevar_ps(y0, _mm256_set_epi32(0, 0, 0, 0, 1, 1, 1, 1));
    y9 = _mm256_permutevar_ps(y0, _mm256_set_epi32(2, 2, 2, 2, 3, 3, 3, 3));

    lake_simd256_write(dest[0], 
        _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(y2, y6),
                                    _mm256_mul_ps(y3, y7)),
                      _mm256_add_ps(_mm256_mul_ps(y4, y8),
                                    _mm256_mul_ps(y5, y9))));

    /* n n n n i i i i */
    /* p p p p k k k k */
    /* m m m m j j j j */
    /* o o o o l l l l */
    y6 = _mm256_permutevar_ps(y1, _mm256_set_epi32(1, 1, 1, 1, 0, 0, 0, 0));
    y7 = _mm256_permutevar_ps(y1, _mm256_set_epi32(3, 3, 3, 3, 2, 2, 2, 2));
    y8 = _mm256_permutevar_ps(y1, _mm256_set_epi32(0, 0, 0, 0, 1, 1, 1, 1));
    y9 = _mm256_permutevar_ps(y1, _mm256_set_epi32(2, 2, 2, 2, 3, 3, 3, 3));

    lake_simd256_write(dest[2], 
        _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(y2, y6),
                                    _mm256_mul_ps(y3, y7)),
                      _mm256_add_ps(_mm256_mul_ps(y4, y8),
                                    _mm256_mul_ps(y5, y9))));
}
#endif /* LAKE_ARCH_X86_AVX */

#if defined(LAKE_ARCH_X86_SSE2)
void lake_mat4_scale_sse2(mat4 m, float s)
{
    f128 x0;
    x0 = _mm_set1_ps(s);

    lake_simd_write(m[0], _mm_mul_ps(lake_simd_read(m[0]), x0));
    lake_simd_write(m[1], _mm_mul_ps(lake_simd_read(m[1]), x0));
    lake_simd_write(m[2], _mm_mul_ps(lake_simd_read(m[2]), x0));
    lake_simd_write(m[3], _mm_mul_ps(lake_simd_read(m[3]), x0));
}

void lake_mat4_transp_sse2(mat4 m, mat4 dest) 
{
    f128 r0, r1, r2, r3;

    r0 = lake_simd_read(m[0]);
    r1 = lake_simd_read(m[1]);
    r2 = lake_simd_read(m[2]);
    r3 = lake_simd_read(m[3]);

    _MM_TRANSPOSE4_PS(r0, r1, r2, r3);

    lake_simd_write(dest[0], r0);
    lake_simd_write(dest[1], r1);
    lake_simd_write(dest[2], r2);
    lake_simd_write(dest[3], r3);
}

void lake_mat4_mul_sse2(mat4 m1, mat4 m2, mat4 dest)
{
    /* D = R * L (column-major) */
    f128 l, r0, r1, r2, r3, v0, v1, v2, v3;

    l  = lake_simd_read(m1[0]);
    r0 = lake_simd_read(m2[0]);
    r1 = lake_simd_read(m2[1]);
    r2 = lake_simd_read(m2[2]);
    r3 = lake_simd_read(m2[3]);

    v0 = _mm_mul_ps(lake_simd_splat_x(r0), l);
    v1 = _mm_mul_ps(lake_simd_splat_x(r1), l);
    v2 = _mm_mul_ps(lake_simd_splat_x(r2), l);
    v3 = _mm_mul_ps(lake_simd_splat_x(r3), l);

    l  = lake_simd_read(m1[1]);
    v0 = lake_simd_fmadd(lake_simd_splat_y(r0), l, v0);
    v1 = lake_simd_fmadd(lake_simd_splat_y(r1), l, v1);
    v2 = lake_simd_fmadd(lake_simd_splat_y(r2), l, v2);
    v3 = lake_simd_fmadd(lake_simd_splat_y(r3), l, v3);

    l  = lake_simd_read(m1[2]);
    v0 = lake_simd_fmadd(lake_simd_splat_z(r0), l, v0);
    v1 = lake_simd_fmadd(lake_simd_splat_z(r1), l, v1);
    v2 = lake_simd_fmadd(lake_simd_splat_z(r2), l, v2);
    v3 = lake_simd_fmadd(lake_simd_splat_z(r3), l, v3);

    l  = lake_simd_read(m1[3]);
    v0 = lake_simd_fmadd(lake_simd_splat_w(r0), l, v0);
    v1 = lake_simd_fmadd(lake_simd_splat_w(r1), l, v1);
    v2 = lake_simd_fmadd(lake_simd_splat_w(r2), l, v2);
    v3 = lake_simd_fmadd(lake_simd_splat_w(r3), l, v3);

    lake_simd_write(dest[0], v0);
    lake_simd_write(dest[1], v1);
    lake_simd_write(dest[2], v2);
    lake_simd_write(dest[3], v3);
}

void lake_mat4_mulv_sse2(mat4 m, vec4 v, vec4 dest)
{
    f128 x0, x1, m0, m1, m2, m3, v0, v1, v2, v3;

    m0 = lake_simd_read(m[0]);
    m1 = lake_simd_read(m[1]);
    m2 = lake_simd_read(m[2]);
    m3 = lake_simd_read(m[3]);

    x0 = lake_simd_read(v);
    v0 = lake_simd_splat_x(x0);
    v1 = lake_simd_splat_y(x0);
    v2 = lake_simd_splat_z(x0);
    v3 = lake_simd_splat_w(x0);

    x1 = _mm_mul_ps(m3, v3);
    x1 = lake_simd_fmadd(m2, v2, x1);
    x1 = lake_simd_fmadd(m1, v1, x1);
    x1 = lake_simd_fmadd(m0, v0, x1);

    lake_simd_write(dest, x1);
}

f32 lake_mat4_det_sse2(mat4 m)
{
    f128 r0, r1, r2, r3, x0, x1, x2;

    /* 127 <- 0, [square] det(A) = det(At) */
    r0 = lake_simd_read(m[0]); /* d c b a */
    r1 = lake_simd_read(m[1]); /* h g f e */
    r2 = lake_simd_read(m[2]); /* l k j i */
    r3 = lake_simd_read(m[3]); /* p o n m */

    /* t[1] = j * p - n * l;
     * t[2] = j * o - n * k;
     * t[3] = i * p - m * l;
     * t[4] = i * o - m * k; */
    x0 = lake_simd_fnmadd(lake_simd_shuffle1(r3, 0, 0, 1, 1), lake_simd_shuffle1(r2, 2, 3, 2, 3),
                   _mm_mul_ps(lake_simd_shuffle1(r2, 0, 0, 1, 1),
                              lake_simd_shuffle1(r3, 2, 3, 2, 3)));
    /* t[0] = k * p - o * l;
     * t[0] = k * p - o * l;
     * t[5] = i * n - m * j;
     * t[5] = i * n - m * j; */
    x1 = lake_simd_fnmadd(lake_simd_shuffle1(r3, 0, 0, 2, 2), lake_simd_shuffle1(r2, 1, 1, 3, 3),
                   _mm_mul_ps(lake_simd_shuffle1(r2, 0, 0, 2, 2),
                              lake_simd_shuffle1(r3, 1, 1, 3, 3)));

    /*     a * (f * t[0] - g * t[1] + h * t[2])
     *   - b * (e * t[0] - g * t[3] + h * t[4])
     *   + c * (e * t[1] - f * t[3] + h * t[5])
     *   - d * (e * t[2] - f * t[4] + g * t[5]) */
    x2 = lake_simd_fnmadd(lake_simd_shuffle1(r1, 1, 1, 2, 2), lake_simd_shuffle1(x0, 3, 2, 2, 0),
                   _mm_mul_ps(lake_simd_shuffle1(r1, 0, 0, 0, 1),
                              _mm_shuffle_ps(x1, x0, _MM_SHUFFLE(1, 0, 0, 0))));
    x2 = lake_simd_fmadd(lake_simd_shuffle1(r1, 2, 3, 3, 3),
                  _mm_shuffle_ps(x0, x1, _MM_SHUFFLE(2, 2, 3, 1)), x2);

    x2 = _mm_xor_ps(x2, lake_simd_float32x4_SIGNMASK_NPNP);

    return lake_simd_hadd(_mm_mul_ps(x2, r0));
}

void lake_mat4_inv_fast_sse2(mat4 m, mat4 dest)
{
    f128 r0, r1, r2, r3,
         v0, v1, v2, v3,
         t0, t1, t2, t3, t4, t5,
         x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;

    /* x8 = _mm_set_ps(-0.f, 0.f, -0.f, 0.f); */
    x8 = lake_simd_float32x4_SIGNMASK_NPNP;
    x9 = lake_simd_shuffle1(x8, 2, 1, 2, 1);

    /* 127 <- 0 */
    r0 = lake_simd_read(m[0]); /* d c b a */
    r1 = lake_simd_read(m[1]); /* h g f e */
    r2 = lake_simd_read(m[2]); /* l k j i */
    r3 = lake_simd_read(m[3]); /* p o n m */

    x0 = _mm_movehl_ps(r3, r2);                            /* p o l k */
    x3 = _mm_movelh_ps(r2, r3);                            /* n m j i */
    x1 = lake_simd_shuffle1(x0, 1, 3, 3 ,3);               /* l p p p */
    x2 = lake_simd_shuffle1(x0, 0, 2, 2, 2);               /* k o o o */
    x4 = lake_simd_shuffle1(x3, 1, 3, 3, 3);               /* j n n n */
    x7 = lake_simd_shuffle1(x3, 0, 2, 2, 2);               /* i m m m */

    x6 = _mm_shuffle_ps(r2, r1, _MM_SHUFFLE(0, 0, 0, 0));  /* e e i i */
    x5 = _mm_shuffle_ps(r2, r1, _MM_SHUFFLE(1, 1, 1, 1));  /* f f j j */
    x3 = _mm_shuffle_ps(r2, r1, _MM_SHUFFLE(2, 2, 2, 2));  /* g g k k */
    x0 = _mm_shuffle_ps(r2, r1, _MM_SHUFFLE(3, 3, 3, 3));  /* h h l l */

    t0 = _mm_mul_ps(x3, x1);
    t1 = _mm_mul_ps(x5, x1);
    t2 = _mm_mul_ps(x5, x2);
    t3 = _mm_mul_ps(x6, x1);
    t4 = _mm_mul_ps(x6, x2);
    t5 = _mm_mul_ps(x6, x4);

    /* t1[0] = k * p - o * l;
     * t1[0] = k * p - o * l;
     * t2[0] = g * p - o * h;
     * t3[0] = g * l - k * h; */
    t0 = lake_simd_fnmadd(x2, x0, t0);

    /* t1[1] = j * p - n * l;
     * t1[1] = j * p - n * l;
     * t2[1] = f * p - n * h;
     * t3[1] = f * l - j * h; */
    t1 = lake_simd_fnmadd(x4, x0, t1);

    /* t1[2] = j * o - n * k
     * t1[2] = j * o - n * k;
     * t2[2] = f * o - n * g;
     * t3[2] = f * k - j * g; */
    t2 = lake_simd_fnmadd(x4, x3, t2);

    /* t1[3] = i * p - m * l;
     * t1[3] = i * p - m * l;
     * t2[3] = e * p - m * h;
     * t3[3] = e * l - i * h; */
    t3 = lake_simd_fnmadd(x7, x0, t3);

    /* t1[4] = i * o - m * k;
     * t1[4] = i * o - m * k;
     * t2[4] = e * o - m * g;
     * t3[4] = e * k - i * g; */
    t4 = lake_simd_fnmadd(x7, x3, t4);

    /* t1[5] = i * n - m * j;
     * t1[5] = i * n - m * j;
     * t2[5] = e * n - m * f;
     * t3[5] = e * j - i * f; */
    t5 = lake_simd_fnmadd(x7, x5, t5);

    x4 = _mm_movelh_ps(r0, r1);                 /* f e b a */
    x5 = _mm_movehl_ps(r1, r0);                 /* h g d c */

    x0 = lake_simd_shuffle1(x4, 0, 0, 0, 2);    /* a a a e */
    x1 = lake_simd_shuffle1(x4, 1, 1, 1, 3);    /* b b b f */
    x2 = lake_simd_shuffle1(x5, 0, 0, 0, 2);    /* c c c g */
    x3 = lake_simd_shuffle1(x5, 1, 1, 1, 3);    /* d d d h */

    v2 = _mm_mul_ps(x0, t1);
    v1 = _mm_mul_ps(x0, t0);
    v3 = _mm_mul_ps(x0, t2);
    v0 = _mm_mul_ps(x1, t0);

    v2 = lake_simd_fnmadd(x1, t3, v2);
    v3 = lake_simd_fnmadd(x1, t4, v3);
    v0 = lake_simd_fnmadd(x2, t1, v0);
    v1 = lake_simd_fnmadd(x2, t3, v1);

    v3 = lake_simd_fmadd(x2, t5, v3);
    v0 = lake_simd_fmadd(x3, t2, v0);
    v2 = lake_simd_fmadd(x3, t5, v2);
    v1 = lake_simd_fmadd(x3, t4, v1);

    /* dest[0][0] =  f * t1[0] - g * t1[1] + h * t1[2];
     * dest[0][1] =-(b * t1[0] - c * t1[1] + d * t1[2]);
     * dest[0][2] =  b * t2[0] - c * t2[1] + d * t2[2];
     * dest[0][3] =-(b * t3[0] - c * t3[1] + d * t3[2]); */
    v0 = _mm_xor_ps(v0, x8);

    /* dest[2][0] =  e * t1[1] - f * t1[3] + h * t1[5];
     * dest[2][1] =-(a * t1[1] - b * t1[3] + d * t1[5]);
     * dest[2][2] =  a * t2[1] - b * t2[3] + d * t2[5];
     * dest[2][3] =-(a * t3[1] - b * t3[3] + d * t3[5]); */
    v2 = _mm_xor_ps(v2, x8);

    /* dest[1][0] =-(e * t1[0] - g * t1[3] + h * t1[4]);
     * dest[1][1] =  a * t1[0] - c * t1[3] + d * t1[4];
     * dest[1][2] =-(a * t2[0] - c * t2[3] + d * t2[4]);
     * dest[1][3] =  a * t3[0] - c * t3[3] + d * t3[4]; */
    v1 = _mm_xor_ps(v1, x9);

    /* dest[3][0] =-(e * t1[2] - f * t1[4] + g * t1[5]);
     * dest[3][1] =  a * t1[2] - b * t1[4] + c * t1[5];
     * dest[3][2] =-(a * t2[2] - b * t2[4] + c * t2[5]);
     * dest[3][3] =  a * t3[2] - b * t3[4] + c * t3[5]; */
    v3 = _mm_xor_ps(v3, x9);

    /* determinant */
    x0 = _mm_shuffle_ps(v0, v1, _MM_SHUFFLE(0, 0, 0, 0));
    x1 = _mm_shuffle_ps(v2, v3, _MM_SHUFFLE(0, 0, 0, 0));
    x0 = _mm_shuffle_ps(x0, x1, _MM_SHUFFLE(2, 0, 2, 0));

    x0 = _mm_rcp_ps(lake_simd_vhadd(_mm_mul_ps(x0, r0)));

    lake_simd_write(dest[0], _mm_mul_ps(v0, x0));
    lake_simd_write(dest[1], _mm_mul_ps(v1, x0));
    lake_simd_write(dest[2], _mm_mul_ps(v2, x0));
    lake_simd_write(dest[3], _mm_mul_ps(v3, x0));
}

void lake_mat4_inv_sse2(mat4 m, mat4 dest)
{
    f128 r0, r1, r2, r3,
         v0, v1, v2, v3,
         t0, t1, t2, t3, t4, t5,
         x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;

    /* x8 = _mm_set_ps(-0.f, 0.f, -0.f, 0.f); */
    x8 = lake_simd_float32x4_SIGNMASK_NPNP;
    x9 = lake_simd_shuffle1(x8, 2, 1, 2, 1);

    /* 127 <- 0 */
    r0 = lake_simd_read(m[0]); /* d c b a */
    r1 = lake_simd_read(m[1]); /* h g f e */
    r2 = lake_simd_read(m[2]); /* l k j i */
    r3 = lake_simd_read(m[3]); /* p o n m */

    x0 = _mm_movehl_ps(r3, r2);                            /* p o l k */
    x3 = _mm_movelh_ps(r2, r3);                            /* n m j i */
    x1 = lake_simd_shuffle1(x0, 1, 3, 3 ,3);               /* l p p p */
    x2 = lake_simd_shuffle1(x0, 0, 2, 2, 2);               /* k o o o */
    x4 = lake_simd_shuffle1(x3, 1, 3, 3, 3);               /* j n n n */
    x7 = lake_simd_shuffle1(x3, 0, 2, 2, 2);               /* i m m m */

    x6 = _mm_shuffle_ps(r2, r1, _MM_SHUFFLE(0, 0, 0, 0));  /* e e i i */
    x5 = _mm_shuffle_ps(r2, r1, _MM_SHUFFLE(1, 1, 1, 1));  /* f f j j */
    x3 = _mm_shuffle_ps(r2, r1, _MM_SHUFFLE(2, 2, 2, 2));  /* g g k k */
    x0 = _mm_shuffle_ps(r2, r1, _MM_SHUFFLE(3, 3, 3, 3));  /* h h l l */

    t0 = _mm_mul_ps(x3, x1);
    t1 = _mm_mul_ps(x5, x1);
    t2 = _mm_mul_ps(x5, x2);
    t3 = _mm_mul_ps(x6, x1);
    t4 = _mm_mul_ps(x6, x2);
    t5 = _mm_mul_ps(x6, x4);

    /* t1[0] = k * p - o * l;
     * t1[0] = k * p - o * l;
     * t2[0] = g * p - o * h;
     * t3[0] = g * l - k * h; */
    t0 = lake_simd_fnmadd(x2, x0, t0);

    /* t1[1] = j * p - n * l;
     * t1[1] = j * p - n * l;
     * t2[1] = f * p - n * h;
     * t3[1] = f * l - j * h; */
    t1 = lake_simd_fnmadd(x4, x0, t1);

    /* t1[2] = j * o - n * k
     * t1[2] = j * o - n * k;
     * t2[2] = f * o - n * g;
     * t3[2] = f * k - j * g; */
    t2 = lake_simd_fnmadd(x4, x3, t2);

    /* t1[3] = i * p - m * l;
     * t1[3] = i * p - m * l;
     * t2[3] = e * p - m * h;
     * t3[3] = e * l - i * h; */
    t3 = lake_simd_fnmadd(x7, x0, t3);

    /* t1[4] = i * o - m * k;
     * t1[4] = i * o - m * k;
     * t2[4] = e * o - m * g;
     * t3[4] = e * k - i * g; */
    t4 = lake_simd_fnmadd(x7, x3, t4);

    /* t1[5] = i * n - m * j;
     * t1[5] = i * n - m * j;
     * t2[5] = e * n - m * f;
     * t3[5] = e * j - i * f; */
    t5 = lake_simd_fnmadd(x7, x5, t5);

    x4 = _mm_movelh_ps(r0, r1);                 /* f e b a */
    x5 = _mm_movehl_ps(r1, r0);                 /* h g d c */

    x0 = lake_simd_shuffle1(x4, 0, 0, 0, 2);    /* a a a e */
    x1 = lake_simd_shuffle1(x4, 1, 1, 1, 3);    /* b b b f */
    x2 = lake_simd_shuffle1(x5, 0, 0, 0, 2);    /* c c c g */
    x3 = lake_simd_shuffle1(x5, 1, 1, 1, 3);    /* d d d h */

    v2 = _mm_mul_ps(x0, t1);
    v1 = _mm_mul_ps(x0, t0);
    v3 = _mm_mul_ps(x0, t2);
    v0 = _mm_mul_ps(x1, t0);

    v2 = lake_simd_fnmadd(x1, t3, v2);
    v3 = lake_simd_fnmadd(x1, t4, v3);
    v0 = lake_simd_fnmadd(x2, t1, v0);
    v1 = lake_simd_fnmadd(x2, t3, v1);

    v3 = lake_simd_fmadd(x2, t5, v3);
    v0 = lake_simd_fmadd(x3, t2, v0);
    v2 = lake_simd_fmadd(x3, t5, v2);
    v1 = lake_simd_fmadd(x3, t4, v1);

    /* dest[0][0] =  f * t1[0] - g * t1[1] + h * t1[2];
     * dest[0][1] =-(b * t1[0] - c * t1[1] + d * t1[2]);
     * dest[0][2] =  b * t2[0] - c * t2[1] + d * t2[2];
     * dest[0][3] =-(b * t3[0] - c * t3[1] + d * t3[2]); */
    v0 = _mm_xor_ps(v0, x8);

    /* dest[2][0] =  e * t1[1] - f * t1[3] + h * t1[5];
     * dest[2][1] =-(a * t1[1] - b * t1[3] + d * t1[5]);
     * dest[2][2] =  a * t2[1] - b * t2[3] + d * t2[5];
     * dest[2][3] =-(a * t3[1] - b * t3[3] + d * t3[5]); */
    v2 = _mm_xor_ps(v2, x8);

    /* dest[1][0] =-(e * t1[0] - g * t1[3] + h * t1[4]);
     * dest[1][1] =  a * t1[0] - c * t1[3] + d * t1[4];
     * dest[1][2] =-(a * t2[0] - c * t2[3] + d * t2[4]);
     * dest[1][3] =  a * t3[0] - c * t3[3] + d * t3[4]; */
    v1 = _mm_xor_ps(v1, x9);

    /* dest[3][0] =-(e * t1[2] - f * t1[4] + g * t1[5]);
     * dest[3][1] =  a * t1[2] - b * t1[4] + c * t1[5];
     * dest[3][2] =-(a * t2[2] - b * t2[4] + c * t2[5]);
     * dest[3][3] =  a * t3[2] - b * t3[4] + c * t3[5]; */
    v3 = _mm_xor_ps(v3, x9);

    /* determinant */
    x0 = _mm_shuffle_ps(v0, v1, _MM_SHUFFLE(0, 0, 0, 0));
    x1 = _mm_shuffle_ps(v2, v3, _MM_SHUFFLE(0, 0, 0, 0));
    x0 = _mm_shuffle_ps(x0, x1, _MM_SHUFFLE(2, 0, 2, 0));

    x0 = _mm_div_ps(_mm_set1_ps(1.0f), lake_simd_vhadd(_mm_mul_ps(x0, r0)));

    lake_simd_write(dest[0], _mm_mul_ps(v0, x0));
    lake_simd_write(dest[1], _mm_mul_ps(v1, x0));
    lake_simd_write(dest[2], _mm_mul_ps(v2, x0));
    lake_simd_write(dest[3], _mm_mul_ps(v3, x0));
}
#endif /* LAKE_ARCH_X86_SSE2 */

void lake_mat4_mul_p(mat4 m1, mat4 m2, mat4 dest)
{
    f32 a00 = m1[0][0], a01 = m1[0][1], a02 = m1[0][2], a03 = m1[0][3],
        a10 = m1[1][0], a11 = m1[1][1], a12 = m1[1][2], a13 = m1[1][3],
        a20 = m1[2][0], a21 = m1[2][1], a22 = m1[2][2], a23 = m1[2][3],
        a30 = m1[3][0], a31 = m1[3][1], a32 = m1[3][2], a33 = m1[3][3],

        b00 = m2[0][0], b01 = m2[0][1], b02 = m2[0][2], b03 = m2[0][3],
        b10 = m2[1][0], b11 = m2[1][1], b12 = m2[1][2], b13 = m2[1][3],
        b20 = m2[2][0], b21 = m2[2][1], b22 = m2[2][2], b23 = m2[2][3],
        b30 = m2[3][0], b31 = m2[3][1], b32 = m2[3][2], b33 = m2[3][3];

    dest[0][0] = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
    dest[0][1] = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
    dest[0][2] = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
    dest[0][3] = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;
    dest[1][0] = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
    dest[1][1] = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
    dest[1][2] = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
    dest[1][3] = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;
    dest[2][0] = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
    dest[2][1] = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
    dest[2][2] = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
    dest[2][3] = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;
    dest[3][0] = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
    dest[3][1] = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
    dest[3][2] = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
    dest[3][3] = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;
}
