#pragma once

/** @file lake/bedrock/endian.h
 *  @brief TODO docs
 */
#include <lake/bedrock/types.h>

#define LAKE_BYTEORDER_LE 1234
#define LAKE_BYTEORDER_BE 4321

#ifndef LAKE_BYTEORDER
    #if defined(LAKE_PLATFORM_LINUX)
        #include <endian.h>
        #define LAKE_BYTEORDER __BYTE_ORDER
    #elif defined(LAKE_PLATFORM_SOLARIS)
        #include <sys/byteorder.h>
        #if defined(_LITTLE_ENDIAN)
            #define LAKE_BYTEORDER LAKE_BYTEORDER_LE
        #elif defined(_BIG_ENDIAN)
            #define LAKE_BYTEORDER LAKE_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #elif defined(LAKE_PLATFORM_OPENBSD) || defined(__DragonFly__)
        #include <endian.h>
        #define LAKE_BYTEORDER BYTE_ORDER
    #elif defined(LAKE_PLATFORM_FREEBSD) || defined(LAKE_PLATFORM_NETBSD)
        #include <sys/endian.h>
        #define LAKE_BYTEORDER BYTE_ORDER
    /* predefs from newer GCC and CLANG versions */
    #elif defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)
        #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
            #define LAKE_BYTEORDER LAKE_BYTEORDER_LE
        #elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define LAKE_BYTEORDER LAKE_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #else  
        #if defined(LAKE_ARCH_MIPS) || defined(LAKE_ARCH_POWER) || defined(LAKE_ARCH_SPARC)
            #define LAKE_BYTEORDER LAKE_BYTEORDER_BE
        #else
            #define LAKE_BYTEORDER LAKE_BYTEORDER_LE
        #endif
    #endif
#endif

LAKE_FORCE_INLINE LAKE_CONST_FN
u16 lake_bswap16(u16 x) 
{
#if LAKE_HAS_BUILTIN(__builtin_bswap16)
    return __builtin_bswap16(x);
#elif LAKE_CC_MSVC_VERSION_CHECK(14,0,0) && defined(LAKE_ARCH_AMD64)
    return _byteswap_ushort(x);
#elif LAKE_CC_MSVC_VERSION_CHECK(14,0,0) && defined(LAKE_ARCH_X86)
    __asm lea  edx, [x]
    __asm mov  ax, [edx]
    __asm xchg ah, al
    __asm mov  [edx], ax
    return x;
#elif defined(LAKE_ARCH_AMD64)
    __asm__("xchgb 0, %h0" : "=Q" (x) : "0" (x));
    return x;
#elif defined(LAKE_ARCH_X86)
    __asm__("rorw $8, %0" : "+r" (x));
    return x;
#elif defined(LAKE_ARCH_POWER)
    int result;
    __asm__("rlwimi %0,%2,8,16,23": "=&r"(result): "0"(x >> 8), "r"(x));
    return lake_static_cast(u16, result);
#else
    return lake_static_cast(u16, ((x << 8) | (x >> 8)));
#endif
}

LAKE_FORCE_INLINE LAKE_CONST_FN
u32 lake_bswap32(u32 x) 
{
#if LAKE_HAS_BUILTIN(__builtin_bswap32)
    return __builtin_bswap32(x);
#elif LAKE_CC_MSVC_VERSION_CHECK(14,0,0) && defined(LAKE_ARCH_AMD64)
    return _byteswap_ulong(x);
#elif LAKE_CC_MSVC_VERSION_CHECK(14,0,0) && defined(LAKE_ARCH_X86)
	__asm lea   edx, [x]
	__asm mov   eax, [edx]
	__asm bswap eax
	__asm mov   [edx], eax
	return x;
#elif defined(LAKE_ARCH_AMD64)
    __asm__("bswapl %0" : "=r" (x) : "0" (x));
    return x;
#elif defined(LAKE_ARCH_X86)
    #if LAKE_ARCH_X86_CHECK(4)
        __asm__("bswap %0" : "+r" (x));
        return x;
    #else /* i386 has broken bswap */
        __asm__("rorw $8, %w0 ; rorl $16, %0 ; rorw $8, %w0" : "+r" (x));
        return x;
    #endif
#elif defined(LAKE_ARCH_POWER)
    u32 result;
    __asm__("rlwimi %0,%2,24,16,23": "=&r"(result): "0"(x >> 24), "r"(x));
    __asm__("rlwimi %0,%2,8,8,15":   "=&r"(result): "0"(result),  "r"(x));
    __asm__("rlwimi %0,%2,24,0,7":   "=&r"(result): "0"(result),  "r"(x));
#else
    return lake_static_cast(u32, ((x << 24) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | (x >> 24)));
#endif
}

LAKE_FORCE_INLINE LAKE_CONST_FN
u64 lake_bswap64(u64 x) 
{
#if LAKE_HAS_BUILTIN(__builtin_bswap64)
    return __builtin_bswap64(x);
#elif LAKE_CC_MSVC_VERSION_CHECK(14,0,0) && defined(LAKE_ARCH_AMD64)
    return _byteswap_uint64(x);
#elif LAKE_CC_MSVC_VERSION_CHECK(14,0,0) && defined(LAKE_ARCH_X86)
	__asm lea   edx, [x]
	__asm mov   eax, [edx]
	__asm bswap eax
	__asm xchg  eax, [edx+4]
	__asm bswap eax
	__asm mov   [edx], eax
	return x;
#elif defined(LAKE_ARCH_AMD64)
    __asm__("bswapq %0" : "=r" (x) : "0" (x));
    return x;
#elif defined(LAKE_ARCH_X86)
    #if LAKE_ARCH_X86_CHECK(4)
        union {
            struct {
                u32 a, b;
            } s;
            u64 u;
        } v;
        v.u = x;
        __asm__("bswapl %0 ; bswapl %1 ; xchgl %0, %1"
                : "=r"(v.s.a), "=r" (v.s.b)
                : "0"(v.s.a), "1" (v.s.b));
    #else /* i386 has broken bswap */
        u32 hi, lo;
        lo = (u32)(x & 0xFFFFFFFF);
        x >>= 32;
        hi = (u32)(x & 0xFFFFFFFF);
        x = bswap32(lo);
        x <<= 32;
        x |= bswap32(hi);
        return x;
    #endif
#else
    u32 hi, lo;
    lo = lake_static_cast(u32, x & 0xffffffff);
    x >>= 32;
    hi = lake_static_cast(u32, x & 0xffffffff);
    x = lake_bswap32(lo);
    x <<= 32;
    x |= lake_bswap32(hi);
    return x;
#endif
}

#if LAKE_HAS_ATTRIBUTE(force) || defined(__CHECKER__)
#define LAKE_ENDIAN_CAST __attribute__((force))
#else
#define LAKE_ENDIAN_CAST
#endif

#if LAKE_HAS_ATTRIBUTE(bitwise) || defined(__CHECKER__)
#define LAKE_ENDIAN_TYPE __attribute__((bitwise))
#else
#define LAKE_ENDIAN_TYPE
#endif

typedef u64 LAKE_ENDIAN_TYPE le64;
typedef u32 LAKE_ENDIAN_TYPE le32;
typedef u16 LAKE_ENDIAN_TYPE le16;

typedef u64 LAKE_ENDIAN_TYPE be64;
typedef u32 LAKE_ENDIAN_TYPE be32;
typedef u16 LAKE_ENDIAN_TYPE be16;

#if LAKE_BYTEORDER == LAKE_BYTEORDER_LE
#define LAKE_LITTLE_ENDIAN 1
#define LAKE_CPU_TO_LE64(native) (lake_static_cast(LAKE_ENDIAN_CAST le64, native))
#define LAKE_CPU_TO_LE32(native) (lake_static_cast(LAKE_ENDIAN_CAST le32, native))
#define LAKE_CPU_TO_LE16(native) (lake_static_cast(LAKE_ENDIAN_CAST le16, native))
#define LAKE_LE64_TO_CPU(le_val) (lake_static_cast(LAKE_ENDIAN_CAST u64, le_val)) 
#define LAKE_LE32_TO_CPU(le_val) (lake_static_cast(LAKE_ENDIAN_CAST u32, le_val)) 
#define LAKE_LE16_TO_CPU(le_val) (lake_static_cast(LAKE_ENDIAN_CAST u16, le_val)) 

#define LAKE_CPU_TO_BE64(native) (lake_static_cast(LAKE_ENDIAN_CAST be64, lake_bswap64(native)))
#define LAKE_CPU_TO_BE32(native) (lake_static_cast(LAKE_ENDIAN_CAST be32, lake_bswap32(native)))
#define LAKE_CPU_TO_BE16(native) (lake_static_cast(LAKE_ENDIAN_CAST be16, lake_bswap16(native)))
#define LAKE_BE64_TO_CPU(be_val) lake_bswap64(lake_static_cast(LAKE_ENDIAN_CAST u64, be_val)) 
#define LAKE_BE32_TO_CPU(be_val) lake_bswap32(lake_static_cast(LAKE_ENDIAN_CAST u32, be_val)) 
#define LAKE_BE16_TO_CPU(be_val) lake_bswap16(lake_static_cast(LAKE_ENDIAN_CAST u16, be_val)) 
#else /* big endian */
#define LAKE_BIG_ENDIAN 1
#define LAKE_CPU_TO_LE64(native) (lake_static_cast(LAKE_ENDIAN_CAST le64, lake_bswap64(native)))
#define LAKE_CPU_TO_LE32(native) (lake_static_cast(LAKE_ENDIAN_CAST le32, lake_bswap32(native)))
#define LAKE_CPU_TO_LE16(native) (lake_static_cast(LAKE_ENDIAN_CAST le16, lake_bswap16(native)))
#define LAKE_LE64_TO_CPU(le_val) lake_bswap64(lake_static_cast(LAKE_ENDIAN_CAST u64, le_val)) 
#define LAKE_LE32_TO_CPU(le_val) lake_bswap32(lake_static_cast(LAKE_ENDIAN_CAST u32, le_val)) 
#define LAKE_LE16_TO_CPU(le_val) lake_bswap16(lake_static_cast(LAKE_ENDIAN_CAST u16, le_val)) 

#define LAKE_CPU_TO_BE64(native) (lake_static_cast(LAKE_ENDIAN_CAST be64, native))
#define LAKE_CPU_TO_BE32(native) (lake_static_cast(LAKE_ENDIAN_CAST be32, native))
#define LAKE_CPU_TO_BE16(native) (lake_static_cast(LAKE_ENDIAN_CAST be16, native))
#define LAKE_BE64_TO_CPU(le_val) (lake_static_cast(LAKE_ENDIAN_CAST u64, be_val)) 
#define LAKE_BE32_TO_CPU(le_val) (lake_static_cast(LAKE_ENDIAN_CAST u32, be_val)) 
#define LAKE_BE16_TO_CPU(le_val) (lake_static_cast(LAKE_ENDIAN_CAST u16, be_val)) 
#endif

/** Convert a uint64_t to little-endian. */
LAKE_FORCE_INLINE le64 lake_cpu_to_le64(u64 native)
{ return LAKE_CPU_TO_LE64(native); }

/** Convert a uint32_t to little-endian. */
LAKE_FORCE_INLINE le32 lake_cpu_to_le32(u32 native)
{ return LAKE_CPU_TO_LE32(native); }

/** Convert a uint16_t to little-endian. */
LAKE_FORCE_INLINE le16 lake_cpu_to_le16(u16 native)
{ return LAKE_CPU_TO_LE16(native); }

/** Convert little-endian to uint64_t. */
LAKE_FORCE_INLINE u64 lake_le64_to_cpu(le64 val)
{ return LAKE_LE64_TO_CPU(val); }

/** Convert little-endian to uint32_t. */
LAKE_FORCE_INLINE u32 lake_le32_to_cpu(le32 val)
{ return LAKE_LE32_TO_CPU(val); }

/** Convert little-endian to uint16_t. */
LAKE_FORCE_INLINE u16 lake_le16_to_cpu(le16 val)
{ return LAKE_LE16_TO_CPU(val); }

/** Convert a uint64_t to big-endian. */
LAKE_FORCE_INLINE be64 lake_cpu_to_be64(u64 native)
{ return LAKE_CPU_TO_BE64(native); }

/** Convert a uint32_t to big-endian. */
LAKE_FORCE_INLINE be32 lake_cpu_to_be32(u32 native)
{ return LAKE_CPU_TO_BE32(native); }

/** Convert a uint16_t to big-endian. */
LAKE_FORCE_INLINE be16 lake_cpu_to_be16(u16 native)
{ return LAKE_CPU_TO_BE16(native); }

/** Convert big-endian to uint64_t. */
LAKE_FORCE_INLINE u64 lake_be64_to_cpu(be64 val)
{ return LAKE_BE64_TO_CPU(val); }

/** Convert big-endian to uint32_t. */
LAKE_FORCE_INLINE u32 lake_be32_to_cpu(be32 val)
{ return LAKE_BE32_TO_CPU(val); }

/** Convert big-endian to uint16_t. */
LAKE_FORCE_INLINE u16 lake_be16_to_cpu(be16 val)
{ return LAKE_BE16_TO_CPU(val); }
