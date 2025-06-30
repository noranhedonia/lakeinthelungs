#include <lake/math/trigonometry.h>

f32 lake_floorf(f32 x)
{
    union { f32 f; u32 i; } u = {x};

    s32 exponent = (s32)(u.i >> 23 & 0xff) - 0x7f;
    u32 mantissa;

    if (exponent >= 23)
        return x;

    if (exponent >= 0) {
        mantissa = 0x007fffff >> exponent;
        if ((u.i & mantissa) == 0)
            return x;
        LAKE_FORCE_EVAL(x + 0x1p120f);
        if (u.i >> 31)
            u.i += mantissa;
        u.i &= ~mantissa;
    } else {
        LAKE_FORCE_EVAL(x + 0x1p120f);
        if (u.i >> 31 == 0)
            u.i = 0;
        else if (u.i << 1)
            u.f = -1.0;
    }
    return u.f;
}

f32 lake_ceilf(f32 x)
{
    union { f32 f; u32 i; } u = {x};

    s32 exponent = (s32)(u.i >> 23 & 0xff) - 0x7f;
    u32 mantissa;

    if (exponent >= 23)
        return x;

    if (exponent >= 0) {
        mantissa = 0x007fffff >> exponent;
        if ((u.i & mantissa) == 0)
            return x;
        LAKE_FORCE_EVAL(x + 0x1p120f);
        if (u.i >> 31 == 0)
            u.i += mantissa;
        u.i &= ~mantissa;
    } else {
        LAKE_FORCE_EVAL(x + 0x1p120f);
        if (u.i >> 31)
            u.f = -0.0;
        else if (u.i << 1)
            u.f = 1.0;
    }
    return u.f;
}

/** The Magic Number fast square root function derived from Quake 3;
 *  For reference visit: www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Funktion-Precisi
 *  alternative magic number: 0x5f375a86 */
f32 lake_sqrtf(f32 x)
{
    f32 const xhalf = 0.5f * x;
    union { f32 f; s32 i; } u;
    u.f = x;
    u.i = 0x5f3759df - (u.i >> 1);
    return x * u.f * (1.5f - xhalf * u.f * u.f);
}

static constexpr f32 g_B = 4.f / LAKE_PIf;
static constexpr f32 g_C = -4.f / (LAKE_PIf * LAKE_PIf);
static constexpr f32 g_P = 0.225f;

f32 lake_sinf(f32 x)
{
    f32 const y = g_B * x + g_C * x * (x < 0 ? -x : x);
    return g_P * (y * (y < 0 ? -y : y) - y) + y;
}

f32 lake_cosf(f32 x)
{
    x = (x > 0) ? -x : x;
    x += LAKE_PI_2f;
    return lake_sinf(x);
}
