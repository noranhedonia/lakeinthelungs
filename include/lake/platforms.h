#pragma once

#if defined(unix) || defined(__unix) || defined(__unix__)
#define LAKE_PLATFORM_UNIX 1
#endif

#if defined(linux) || defined(__linux) || defined(__linux__)
#define LAKE_PLATFORM_LINUX 1
#endif

#if defined(ANDROID) || defined(__ANDROID__)
#define LAKE_PLATFORM_ANDROID 1
#endif

#if defined(EMSCRIPTEN) || defined(__EMSCRIPTEN__)
#define LAKE_PLATFORM_EMSCRIPTEN 1
#endif

#if defined(bsdi) || defined(__bsdi) || defined(__bsdi__)
#define LAKE_PLATFORM_BSDI 1
#endif

#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__DragonFly__)
#define LAKE_PLATFORM_FREEBSD 1
#endif

#ifdef __NetBSD__
#define LAKE_PLATFORM_NETBSD 1
#endif

#ifdef __OpenBSD__
#define LAKE_PLATFORM_OPENBSD 1
#endif

#ifdef __HAIKU__
#define LAKE_PLATFORM_HAIKU 1
#endif

#if defined(__APPLE__)
#define LAKE_PLATFORM_APPLE 1
#include <AvailabilityMacros.h>

#ifndef __has_extension
    #define __has_extension(x) 0
    #include <TargetConditionals.h>
    #undef __has_extension
#else
    #include <TargetConditionals.h>
#endif

#ifdef TARGET_OS_VISION
#define LAKE_PLATFORM_VISIONOS 1
#endif
#ifdef TARGET_OS_IPHONE
#define LAKE_PLATFORM_IOS 1
#else
#define LAKE_PLATFORM_MACOS 1
#endif
#endif /* APPLE */

#ifdef __CYGWIN__
#define LAKE_PLATFORM_CYGWIN 1
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(LAKE_PLATFORM_CYGWIN)
#if defined(_MSC_VER) && defined(__has_include)
    #if __has_include(<winapifamily.h>)
        #define HAVE_WINAPIFAMILY_H 1
    #else
        #define HAVE_WINAPIFAMILY_H 0
    #endif
#elif defined(_MSC_VER) && (_MSC_VER >= 1700 && !_USING_V110_SDK71_)
    #define HAVE_WINAPIFAMILY_H 1
#else
    #define HAVE_WINAPIFAMILY_H 0
#endif /* MSVC */
#if HAVE_WINAPIFAMILY_H
    #define WINAPI_FAMILY_WINRT (!WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP))
#else
    #define WINAPI_FAMILY_WINRT 0
#endif

#if WINAPI_FAMILY_WINRT
    #error Windows RT/UWP is unsupported.
#elif defined(_GAMING_DESKTOP)
    #define LAKE_PLATFORM_WINGDK 1
#elif defined(_GAMING_XBOX_XBOXONE)
    #define LAKE_PLATFORM_XBOXONE 1
#elif defined(_GAMING_XBOX_SCARLETT)
    #define LAKE_PLATFORM_XBOXSERIES 1
#else 
#define LAKE_PLATFORM_WIN32 1 /* desktop windows */
#endif
#if defined(LAKE_PLATFORM_WINGDK) || defined(LAKE_PLATFORM_XBOXONE) || defined(LAKE_PLATFORM_XBOXSERIES)
#define LAKE_PLATFORM_GDK 1
#endif
#endif /* WINDOWS */
