#pragma once

/** @file lake/bedrock/simd.h
 *  @brief Wrapper to unify common SIMD operations on different architectures.
 *
 *  SIMD stands for "single instruction, multiple data". I exploit architecture-specific SIMD instructions
 *  to augment data transformations and math (mostly linear algebra). The headers below define a common
 *  abstraction layer for most commonly used operations across multiple target architectures. This is not 
 *  an effort to completely enable portable vectorized math, instead it's an utility for writing such math.
 *  If `LAKE_SIMD_UNALIGNED` is defined, the SIMD macros will not expect the data to be aligned properly.
 *
 *  Supported targets include:
 *  - x86/amd64 (SSE, SSE2, SSE3, SSSE3, SSE4_1, SSE4_2, AVX, AVX2, FMA)
 *  - arm/aarch64 (NEON)
 *  - RISC-V (Vector extension, e.g. rv64gvc)
 *  - WebAssembly
 *
 *  If any of these target's implementation headers are defined here, LAKE_SIMD is set to 1. Each backend 
 *  defines 128-bit (4x32-bit) types for float (f128) and signed int (s128) types, and common operations 
 *  to work with them. These operations require 16-byte alignment of vector data. The abstraction layer
 *  is mostly defined for single-precision floating point operations to work with the f128 type.
 *
 *  Sign masks are always defined:
 *      lake_simd_float32x4_SIGNMASK_PNPN
 *      lake_simd_float32x4_SIGNMASK_NPNP
 *      lake_simd_float32x4_SIGNMASK_NPPN
 *
 *  Operations defined by all implementations:
 *      lake_simd_read          Moves packed f128 to a destination vector.
 *      lake_simd_write         Stores 4 values packed into a f128 destination vector.
 *      lake_simd_shuffle1      Shuffles lanes in a 128-bit vector (4 lanes).
 *      lake_simd_shuffle2      Shuffles lanes, but between two 128-bit vectors.
 *      lake_simd_splat         Broadcasts a lane in the 128-bit vector into all other lanes.
 *      lake_simd_splat_x       Broadcast the X(0) lane.
 *      lake_simd_splat_y       Broadcast the Y(1) lane.
 *      lake_simd_splat_z       Broadcast the Z(2) lane.
 *      lake_simd_splat_w       Broadcast the W(3) lane.
 *      lake_simd_set1          Initializes the entire 128-bit vector with a scalar float.
 *      lake_simd_set1_ptr      Like lake_simd_set1 but from a pointer to the scalar float.
 *      lake_simd_set1_rval     Like lake_simd_set1 but the scalar flaot is an r-value.
 *      lake_simd_abs           Absolute value of each lane in the 128-bit vector.
 *      lake_simd_min           Minimum value of each lane between two 128-bit vectors.
 *      lake_simd_max           Maximum value of each lane between two 128-bit vectors.
 *      lake_simd_vhadd         Returns a 128-bit vector after a horizontal addition of 4 lanes in a vector.
 *      lake_simd_hadd          Returns a float after a horizontal addition of 4 lanes in a vector.
 *      lake_simd_hmin          Returns a float with a minimum value in a vector.
 *      lake_simd_hmax          Returns a float with a maximum value in a vector.
 *      lake_simd_vdot          Returns a 128-bit vector with every lane having the dot product of another vector.
 *      lake_simd_dot           Returns a float that is the dot product of a vecter.
 *      lake_simd_norm          The euclidean norm (L2): sqrt(a * a).
 *      lake_simd_norm2         The squared L2 norm: (a * a).
 *      lake_simd_norm_one      The L1 norm: sum(abs(a)).
 *      lake_simd_norm_inf      The infinity norm: max(abs(a)).
 *      lake_simd_div           Performs a lane-wise floating-point division of two 128-bit vectors.
 *      lake_simd_fmadd         Computes (a * b + c).
 *      lake_simd_fnmadd        Computes (-a * b + c).
 *      lake_simd_fmsub         Computes (a * b - c).
 *      lake_simd_fnmsub        Computes (-a * b - c).
 *
 *  And if the 256-bit vector f256 is defined:
 *      lake_simd256_read       Moves packed f256 to a destination vector.
 *      lake_simd256_write      Stores 4 values packed into a f256 destination vector.
 *      lake_simd256_fmadd      Computes (a * b + c).
 *      lake_simd256_fnmadd     Computes (-a * b + c).
 *      lake_simd256_fmsub      Computes (a * b - c).
 *      lake_simd256_fnmsub     Computes (-a * b - c).
 */
#include <lake/bedrock/types.h>

/* do not use SIMD alignment for older visual studio versions */
#if defined(LAKE_CC_MSVC_VERSION) && !LAKE_CC_MSVC_VERSION_CHECK(19,13,0)
    /* Visual Studio 2017 version 15.6 */
    #ifndef LAKE_SIMD_UNALIGNED
        #define LAKE_SIMD_UNALIGNED
    #endif
#endif 

#define LAKE_SIMD_H
#if defined(LAKE_DISABLE_SIMD)
    /* ignored */
#elif defined(LAKE_ARCH_X86) || defined(LAKE_ARCH_AMD64)
    #include <lake/bedrock/private/simd_x86.h>
#elif defined(LAKE_ARCH_ARM) || defined(LAKE_ARCH_AARCH64)
    #include <lake/bedrock/private/simd_arm.h>
#elif defined(LAKE_ARCH_RISCV)
    #include <lake/bedrock/private/simd_riscv.h>
#elif defined(LAKE_ARCH_WASM)
    #include <lake/bedrock/private/simd_wasm.h>
#endif
#undef LAKE_SIMD_H

#if defined(LAKE_SIMD_X86) || defined(LAKE_SIMD_ARM) || defined(LAKE_SIMD_RISCV) || defined(LAKE_SIMD_WASM)
#define LAKE_SIMD 1
#else
#define LAKE_SIMD 0
#endif
