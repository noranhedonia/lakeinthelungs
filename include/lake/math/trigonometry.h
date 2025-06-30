#pragma once

/** @file lake/math/trigonometry.h
 *  @brief TODO docs
 */
#include <lake/bedrock/simd.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

LAKE_FORCE_INLINE
void lake_force_evalf(f32 x) 
{ volatile f32 y; y = x; (void)y; }

LAKE_FORCE_INLINE
void lake_force_eval(f64 x) 
{ volatile f64 y; y = x; (void)y; }

LAKE_FORCE_INLINE
void lake_force_evall(long double x) 
{ volatile long double y; y = x; (void)y; }

#define LAKE_FORCE_EVAL(x) do {                 \
        if (sizeof(x) == sizeof(f32)) {         \
            lake_force_evalf(x);                \
        } else if (sizeof(x) == sizeof(f64)) {  \
            lake_force_eval(x);                 \
        } else {                                \
            lake_force_evall(x);                \
        }                                       \
    } while(0)

/* XXX move out of here? */
LAKEAPI f32 LAKECALL lake_floorf(f32 x);

/* XXX move out of here? */
LAKEAPI f32 LAKECALL lake_ceilf(f32 x);

/* XXX move out of here? */
LAKEAPI f32 LAKECALL lake_sqrtf(f32 x);

/** Fast approximation of Sin. */
LAKEAPI LAKE_HOT_FN LAKE_CONST_FN
f32 LAKECALL lake_sinf(f32 x);

/** Fast approximation of Cos. */
LAKEAPI LAKE_HOT_FN LAKE_CONST_FN 
f32 LAKECALL lake_cosf(f32 x);

#ifdef __cplusplus
}
#endif /* __cplusplus */
