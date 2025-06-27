#include <lake/math/quat.h>

#if defined(LAKE_ARCH_X86_SSE2)
void lake_quat_mul_sse2(quat p, quat q, quat dest)
{
    /* (a1 b2 + b1 a2 + c1 d2 - d1 c2)i
     * (a1 c2 - b1 d2 + c1 a2 + d1 b2)j
     * (a1 d2 + b1 c2 - c1 b2 + d1 a2)k
     *  a1 a2 - b1 b2 - c1 c2 - d1 d2 */
    f128 xp, xq, x1, x2, x3, r, x, y, z;

    xp = lake_simd_read(p); /* 3 2 1 0 */
    xq = lake_simd_read(q);
    x1 = lake_simd_float32x4_SIGNMASK_NPNP; /* XXX _mm_set1_ss() + shuff ? */
    r  = _mm_mul_ps(lake_simd_splat_w(xp), xq);

    x2 = _mm_unpackhi_ps(x1, x1);
    x3 = lake_simd_shuffle1(x1, 3, 2, 0, 1);
    x  = lake_simd_splat_x(xp);
    y  = lake_simd_splat_y(xp);
    z  = lake_simd_splat_z(xp);

    x  = _mm_xor_ps(x, x1);
    y  = _mm_xor_ps(y, x2);
    z  = _mm_xor_ps(z, x3);

    x1 = lake_simd_shuffle1(xq, 0, 1, 2, 3);
    x2 = lake_simd_shuffle1(xq, 1, 0, 3, 2);
    x3 = lake_simd_shuffle1(xq, 2, 3, 0, 1);

    r  = lake_simd_fmadd(x, x1, r);
    r  = lake_simd_fmadd(y, x2, r);
    r  = lake_simd_fmadd(z, x3, r);

    lake_simd_write(dest, r);
}
#endif /* LAKE_ARCH_X86_SSE2 */

void lake_quat_from_mat4(mat4 m, quat dest)
{
    f32 trace, r, rinv;

    trace = m[0][0] + m[1][1] + m[2][2];
    if (trace >= 0.0f) {
        r = sqrtf(1.0f + trace);
        rinv = 0.5f / r;
        dest[0] = rinv * (m[1][2] - m[2][1]);
        dest[1] = rinv * (m[2][0] - m[0][2]);
        dest[2] = rinv * (m[0][1] - m[1][0]);
        dest[3] =    r * 0.5f;
    } else if (m[0][0] >= m[1][1] && m[0][0] >= m[2][2]) {
        r = sqrtf(1.0f - m[1][1] - m[2][2] + m[0][0]);
        rinv = 0.5f / r;
        dest[0] =    r * 0.5f;
        dest[1] = rinv * (m[0][1] - m[1][0]);
        dest[2] = rinv * (m[0][2] - m[2][0]);
        dest[3] = rinv * (m[1][2] - m[2][1]);
    } else if (m[1][1] >= m[2][2]) {
        r = sqrtf(1.0f - m[0][0] - m[2][2] + m[1][1]);
        rinv = 0.5f / r;
        dest[0] = rinv * (m[0][1] - m[1][0]);
        dest[1] =    r * 0.5f;
        dest[2] = rinv * (m[1][2] - m[2][1]);
        dest[3] = rinv * (m[2][0] - m[0][2]);
    } else {
        r = sqrtf(1.0f - m[0][0] - m[1][1] + m[2][2]);
        rinv = 0.5f / r;
        dest[0] = rinv * (m[0][2] - m[2][0]);
        dest[1] = rinv * (m[1][2] - m[2][1]);
        dest[2] =    r * 0.5f;
        dest[1] = rinv * (m[0][1] - m[1][0]);
    }
}
