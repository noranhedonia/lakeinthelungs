#include <lake/math/mat3.h>

#if defined(LAKE_ARCH_X86_SSE2)
void lake_mat2_mul_sse2(mat2 m1, mat2 m2, mat2 dest)
{
    f128 x0, x1, x2, x3, x4;

    x1 = lake_simd_read(m1[0]); /* d c b a */
    x2 = lake_simd_read(m2[0]); /* h g f e */

    x3 = lake_simd_shuffle1(x2, 2, 2, 0, 0);
    x4 = lake_simd_shuffle1(x2, 3, 3, 1, 1);
    x0 = _mm_movelh_ps(x1, x1);
    x2 = _mm_movehl_ps(x1, x1);

    /* dest[0][0] = a * e + c * f;
     * dest[0][1] = b * e + d * f;
     * dest[1][0] = a * g + c * h;
     * dest[1][1] = b * g + d * h; */
    x0 = lake_simd_fmadd(x0, x3, _mm_mul_ps(x2, x4));

    lake_simd_write(dest[0], x0);
}
#endif /* LAKE_ARCH_X86_SSE2 */
