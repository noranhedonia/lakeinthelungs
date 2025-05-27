#pragma once

#include <lake/bedrock/types.h>

#ifdef __cplusplus
#include <ccomplex>
#else
#include <complex.h>
#endif /* __cplusplus */

#ifdef LAKE_CC_MSVC_VERSION
typedef _Fcomplex               cmplxf;
typedef _Dcomplex               cmplx;
typedef _Lcomplex               cmplxl;
#else
typedef _Complex float          cmplxf;
typedef _Complex double         cmplx;
typedef _Complex long double    cmplxl;
#endif

#ifdef LAKE_CC_CLANG_VERSION
    /** On these platforms CMPLX is defined without __extension__, causing false warnings. */
    #if defined(LAKE_PLATFORM_EMSCRIPTEN) || defined(LAKE_PLATFORM_APPLE)
        #undef CMPLXF
        #undef CMPLX
        #undef CMPLXL
    #endif
#endif

#if!defined(CMPLXF)
    #if LAKE_HAS_BUILTIN(__builtin_complex)
        #define CMPLXF(RE, IM) __builtin_complex((float)(RE), (float)(IM))
    #elif defined(LAKE_CC_MSVC_VERSION)
        #define CMPLXF(RE, IM) (_Fcomplex){(float)(RE), (float)(IM)}
    #elif defined(LAKE_CC_CLANG_VERSION)
        #define CMPLXF(RE, IM) (__extension__(_Complex float){(float)(RE), (float)(IM)})
    #elif defined(_Imaginary_I)
        #define CMPLXF(RE, IM) (_Complex float)((float)(RE) + _Imaginary_I * (float)(IM))
    #else
        #define CMPLXF(RE, IM) (_Complex float)((float)(RE) + _Complex_I * (float)(IM))
    #endif
#endif /* CMPLXF */

#if !defined(CMPLX)
    #if LAKE_HAS_BUILTIN(__builtin_complex)
        #define CMPLX(RE, IM) __builtin_complex((double)(RE), (double)(IM))
    #elif defined(LAKE_CC_MSVC_VERSION)
        #define CMPLX(RE, IM) (_Dcomplex){(double)(RE), (double)(IM)}
    #elif defined(LAKE_CC_CLANG_VERSION)
        #define CMPLX(RE, IM) (__extension__(_Complex double){(double)(RE), (double)(IM)})
    #elif defined(_Imaginary_I)
        #define CMPLX(RE, IM) (_Complex double)((double)(RE) + _Imaginary_I * (double)(IM))
    #else
        #define CMPLX(RE, IM) (_Complex double)((double)(RE) + _Complex_I * (double)(IM))
    #endif
#endif /* CMPLX */

#if !defined(CMPLXL)
    #if LAKE_HAS_BUILTIN(__builtin_complex)
        #define CMPLXL(RE, IM) __builtin_complex((long double)(RE), (long double)(IM))
    #elif defined(LAKE_CC_MSVC_VERSION)
        #define CMPLXL(RE, IM) (_Lcomplex){(long double)(RE), (long double)(IM)}
    #elif defined(LAKE_CC_CLANG_VERSION)
        #define CMPLX(RE, IM) (__extension__(_Complex long double){(long double)(RE), (long double)(IM)})
    #elif defined(_Imaginary_I)
        #define CMPLXL(RE, IM) (_Complex long double)((long double)(RE) + _Imaginary_I * (long double)(IM))
    #else
        #define CMPLXL(RE, IM) (_Complex long double)((long double)(RE) + _Complex_I * (long double)(IM))
    #endif
#endif /* CMPLXL */
