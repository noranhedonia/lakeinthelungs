#pragma once

#include <lake/compiler.h>
#include <lake/platforms.h>
#include <lake/arch.h>
#include <lake/alignment.h>

#ifdef __cplusplus
#include <cstddef>
#include <cstdarg>
#include <cstdint>
#include <climits>
#include <cfloat>
#include <cmath>
#else
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#endif /* __cplusplus */

/** Declares API function visibility for DLL builds. */
#if !defined(LAKEAPI)
    #ifdef LAKE_BUILD_DLL_EXPORT
        #if defined(LAKE_PLATFORM_WINDOWS)
            #define LAKEAPI extern __declspec(dllexport)
        #elif defined(LAKE_CC_GNUC_VERSION)
            #define LAKEAPI extern __attribute__((visibility("default")))
        #else
            #define LAKEAPI extern
        #endif
    #else
        #define LAKEAPI extern
    #endif
#endif

/* declares the calling convention */
#if !defined(LAKECALL)
    #ifdef LAKE_PLATFORM_WINDOWS
        /* on windows use the stdcall convention */
        #define LAKECALL __stdcall
    #else
        /* on other platforms use the default calling convention */
        #define LAKECALL
    #endif
#endif

typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef intptr_t    sptr;
typedef uintptr_t   uptr;
typedef ptrdiff_t   ssize;
typedef size_t      usize;

typedef float       f32;
typedef double      f64;

#if LAKE_HAS_MAY_ALIAS
typedef u32   lake_may_alias    alias_u32;
typedef u64   lake_may_alias    alias_u64;
typedef usize lake_may_alias    alias_word;
#else
typedef u32                     alias_u32;
typedef u64                     alias_u64;
typedef usize                   alias_word;
#endif /* LAKE_HAS_MAY_ALIAS */

#define LAKE_EPSILON    DBL_EPSILON
#define LAKE_EPSILONf   FLT_EPSILON

#define LAKE_E          2.71828182845904523536 /* e          */
#define LAKE_LOG2E      1.44269504088896340735 /* log2(e)    */
#define LAKE_LOG10E     0.43429448190325182765 /* log10(e)   */
#define LAKE_LN2        0.69314718055994530941 /* loge(2)    */
#define LAKE_LN10       2.30258509299404568401 /* loge(10)   */
#define LAKE_TAU        6.28318530717958647692 /* 2pi        */
#define LAKE_INV_TAU    0.15915494309189533577 /* 1/2pi      */
#define LAKE_PI         3.14159265358979323846 /* pi         */
#define LAKE_PI_2       1.57079632679489661923 /* pi/2       */
#define LAKE_PI_4       0.78539816339744830961 /* pi/4       */
#define LAKE_1_PI       0.31830988618379067153 /* 1/pi       */
#define LAKE_2_PI       0.63661977236758134307 /* 2/pi       */
#define LAKE_2_SQRTPI   1.12837916709551257389 /* 2/sqrt(pi) */
#define LAKE_SQRT2      1.41421356237309504880 /* sqrt(2)    */
#define LAKE_SQRT1_2    0.70710678118654752440 /* 1/sqrt(2)  */

#define LAKE_Ef         ((f32)LAKE_E)
#define LAKE_LOG2Ef     ((f32)LAKE_LOG2E)
#define LAKE_LOG10Ef    ((f32)LAKE_LOG10E)
#define LAKE_LN2f       ((f32)LAKE_LN2)
#define LAKE_LN10f      ((f32)LAKE_LN10)
#define LAKE_TAUf       ((f32)LAKE_TAU)
#define LAKE_INV_TAUf   ((f32)LAKE_INV_TAU)
#define LAKE_PIf        ((f32)LAKE_PI)
#define LAKE_PI_2f      ((f32)LAKE_PI_2)
#define LAKE_PI_4f      ((f32)LAKE_PI_4)
#define LAKE_1_PIf      ((f32)LAKE_1_PI)
#define LAKE_2_PIf      ((f32)LAKE_2_PI)
#define LAKE_2_SQRTPIf  ((f32)LAKE_2_SQRTPI)
#define LAKE_SQRT2f     ((f32)LAKE_SQRT2)
#define LAKE_SQRT1_2f   ((f32)LAKE_SQRT1_2)

typedef s32                         svec2[2];
typedef s32                         svec3[3];
typedef s32                         svec4[4];

typedef f32                         vec2[2];
typedef f32                         vec3[3];
typedef LAKE_ALIGNMENT(16)  f32     vec4[4];

typedef vec4                        quat;       /* quaternion */

typedef LAKE_ALIGNMENT(16)  vec2    mat2[2];    /* 2x2 matrix */
typedef vec3                        mat2x3[2];
typedef vec4                        mat2x4[2];

typedef vec3                        mat3[3];    /* 3x3 matrix */
typedef vec2                        mat3x2[3];
typedef vec4                        mat3x4[3];

typedef LAKE_SIMD_ALIGNMENT vec4    mat4[4];    /* 4x4 matrix */
typedef vec2                        mat4x2[4];
typedef vec3                        mat4x3[4];

#define lake_ssizeof(T) lake_static_cast(s32, sizeof(T))
#define lake_salignof(T) lake_static_cast(s32, alignof(T))

#define lake_shuffle4(z, y, x, w) (((z) << 6) | ((y) << 4) | ((x) << 2) | (w))
#define lake_shuffle3(z, y, x)    (((z) << 4) | ((y) << 2) | (x))
#define lake_shuffle2(y, x)       (((y) << 2) | (x))

#define lake_offset(o, offset) lake_reinterpret_cast(void *, lake_reinterpret_cast(uptr, o) + offset)
#define lake_offset_t(o, T) lake_offset(o, sizeof(T))

#define lake_elem(ptr, size, index) lake_offset(ptr, (size) * (index))
#define lake_elem_t(o, T, index) lake_elem(o, sizeof(T), index)

#define lake_arraysize(a)       (sizeof(a) / sizeof(a[0]))
#define lake_lengthof(s)        (lake_arraysize(s) - 1)
#define lake_is_pow2(x)         ((x != 0) && ((x & (x - 1)) == 0))
#define lake_swap(a,b)          { lake_typeof(a) temp = a; a = b; b = temp; }
#define lake_min(x,y)           (((x) < (y)) ? (x) : (y))
#define lake_max(x,y)           (((x) > (y)) ? (x) : (y))
#define lake_clamp(x,a,b)       (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#define lake_clamp_zo(x)        (lake_clamp(x, 0, 1))

/** Callback used for sorting values. */
typedef s32 (LAKECALL *PFN_lake_compare_op)(const void *ptr1, const void *ptr2);

/** Callback used for hashing values. */
typedef u64 (LAKECALL *PFN_lake_hash_value_op)(const void *ptr);

typedef struct lake_viewport { 
    f32 x, y;
    f32 width, height; 
    f32 min_depth, max_depth; 
} lake_viewport;

typedef struct lake_offset2d { 
    s32 x, y; 
} lake_offset2d;

typedef struct lake_offset3d { 
    s32 x, y, z; 
} lake_offset3d;

typedef struct lake_extent2d { 
    u32 width, height; 
} lake_extent2d;

typedef struct lake_extent3d { 
    u32 width, height, depth; 
} lake_extent3d;

typedef struct lake_rect2d { 
    lake_offset2d offset; 
    lake_extent2d extent; 
} lake_rect2d;

#define LAKE_SMALL_STRING_CAPACITY 63u
typedef struct lake_small_string { 
    char str[LAKE_SMALL_STRING_CAPACITY]; 
    u8   len; 
} lake_small_string;

#define lake_small_string_cstr(cstr) \
    { cstr, lake_min(lake_arraysize(cstr)-1, LAKE_SMALL_STRING_CAPACITY) }

#define lake_pair(T0, T1) struct { T0 first; T1 second; }

/** Result codes for error checking. */
typedef enum lake_result : s32 {
    LAKE_SUCCESS                                                    = 0,
    LAKE_NOT_READY                                                  = 1,
    LAKE_TIMEOUT                                                    = 2,
    LAKE_RETRY                                                      = 3,
    LAKE_INCOMPLETE                                                 = 4,
    LAKE_SUBOPTIMAL                                                 = 5,
    LAKE_EVENT_SET                                                  = 6,
    LAKE_EVENT_RESET                                                = 7,
    LAKE_THREAD_IDLE                                                = 8,
    LAKE_THREAD_DONE                                                = 9,
    LAKE_OPERATION_DEFERRED                                         = 10,
    LAKE_OPERATION_NOT_DEFERRED                                     = 11,
    LAKE_INVALID_PARAMETERS                                         = 12,
    LAKE_PAYLOAD_MISSING                                            = 13,
    LAKE_HANDLE_STILL_REFERENCED                                    = 14,
    LAKE_VALIDATION_FAILED                                          = 15,
    LAKE_FRAGMENTATION                                              = 16,
    LAKE_COMPILATION_REQUIRED                                       = 17,
    LAKE_DEFER_MISSUSE                                              = 18,
    LAKE_FRAMEWORK_REQUIRED                                         = 2738,
    LAKE_PANIC                                                      = 6969,
    LAKE_ERROR_UNKNOWN                                              = -1,
    LAKE_ERROR_INITIALIZATION_FAILED                                = -2,
    LAKE_ERROR_OUT_OF_HOST_MEMORY                                   = -3,
    LAKE_ERROR_OUT_OF_DEVICE_MEMORY                                 = -4,
    LAKE_ERROR_OUT_OF_POOL_MEMORY                                   = -5,
    LAKE_ERROR_FRAGMENTED_POOL                                      = -6,
    LAKE_ERROR_FORMAT_NOT_SUPPORTED                                 = -7,
    LAKE_ERROR_INCOMPATIBLE_DRIVER                                  = -8,
    LAKE_ERROR_INCOMPATIBLE_DISPLAY                                 = -9,
    LAKE_ERROR_EXTENSION_NOT_PRESENT                                = -13,
    LAKE_ERROR_FEATURE_NOT_PRESENT                                  = -16,
    LAKE_ERROR_LAYER_NOT_PRESENT                                    = -19,
    LAKE_ERROR_NOT_PERMITTED                                        = -20,
    LAKE_ERROR_DEVICE_LOST                                          = -22,
    LAKE_ERROR_SURFACE_LOST                                         = -25,
    LAKE_ERROR_OUT_OF_DATE                                          = -28,
    LAKE_ERROR_OUT_OF_RANGE                                         = -31,
    LAKE_ERROR_MEMORY_MAP_FAILED                                    = -34,
    LAKE_ERROR_NATIVE_WINDOW_IN_USE                                 = -37,
    LAKE_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS                       = -40,
    LAKE_ERROR_INVALID_EXTERNAL_HANDLE                              = -44,
    LAKE_ERROR_INVALID_DEVICE_INDEX                                 = -48,
    LAKE_ERROR_INVALID_QUEUE                                        = -52,
    LAKE_ERROR_INVALID_SHADER                                       = -53,
    LAKE_ERROR_INVALID_ASSEMBLY_DETAILS                             = -54,
    LAKE_ERROR_DEVICE_NOT_SUPPORTED                                 = -55,
    LAKE_ERROR_DEVICE_DOES_NOT_SUPPORT_BUFFER_COUNT                 = -57,
    LAKE_ERROR_DEVICE_DOES_NOT_SUPPORT_TEXTURE_COUNT                = -60,
    LAKE_ERROR_DEVICE_DOES_NOT_SUPPORT_SAMPLER_COUNT                = -64,
    LAKE_ERROR_DEVICE_DOES_NOT_SUPPORT_ACCELERATION_STRUCTURE_COUNT = -68,
    LAKE_ERROR_FULLSCREEN_EXCLUSIVE_MODE_LOST                       = -69,
    LAKE_ERROR_COULD_NOT_QUERY_DEVICE_QUEUE                         = -72,
    LAKE_ERROR_COMPRESSION_EXHAUSTED                                = -73,
    LAKE_ERROR_TOO_MANY_OBJECTS                                     = -76,
    LAKE_ERROR_RAY_TRACING_REQUIRED                                 = -78,
    LAKE_ERROR_INVALID_BUFFER_ID                                    = -79,
    LAKE_ERROR_INVALID_TEXTURE_ID                                   = -80,
    LAKE_ERROR_INVALID_TEXTURE_VIEW_ID                              = -81,
    LAKE_ERROR_INVALID_SAMPLER_ID                                   = -82,
    LAKE_ERROR_INVALID_TLAS_ID                                      = -83,
    LAKE_ERROR_INVALID_BLAS_ID                                      = -84,
    LAKE_ERROR_QUEUE_SCHEDULING_TYPE_MISMATCH                       = -85,
    LAKE_ERROR_EXCEEDED_MAX_BUFFERS                                 = -86,
    LAKE_ERROR_EXCEEDED_MAX_TEXTURES                                = -87,
    LAKE_ERROR_EXCEEDED_MAX_SAMPLERS                                = -88,
    LAKE_ERROR_EXCEEDED_MAX_ACCELERATION_STRUCTURES                 = -89,
    LAKE_ERROR_EXCEEDED_PUSH_CONSTANT_RANGE                         = -90,
} lake_result;
