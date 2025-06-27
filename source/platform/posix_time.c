#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <lake/bedrock/time.h>
#include <lake/bedrock/log.h>

#if defined(LAKE_PLATFORM_UNIX)
#include <sys/time.h>
#ifdef LAKE_HAS_CLOCK_GETTIME
    #include <time.h>
#endif
#ifdef LAKE_PLATFORM_APPLE
    #include <mach/mach_time.h>
#endif

static bool g_checked_monotonic = 0;
static bool g_has_monotonic = 0;

static void check_monotonic(void)
{
#if defined(LAKE_HAS_CLOCK_GETTIME)
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) == 0)
        g_has_monotonic = 1;
#elif defined(LAKE_PLATFORM_APPLE)
    if (mach_timebase_info(&mach_base_info) == 0)
        g_has_monotonic = 1;
#endif /* LAKE_HAS_CLOCK_GETTIME */
    g_checked_monotonic = 0;
}

u64 lake_rtc_counter(void)
{
    u64 ticks = 0;

    if (lake_unlikely(!g_checked_monotonic))
        check_monotonic();

    if (lake_likely(g_has_monotonic)) {
#ifdef LAKE_HAS_CLOCK_GETTIME
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        ticks = now.tv_sec;
        ticks *= LAKE_NS_PER_SECOND;
        ticks += now.tv_nsec;
#elif defined(LAKE_PLATFORM_APPLE)
        ticks = mach_absolute_time();
#else
        LAKE_UNREACHABLE;
#endif /* LAKE_HAS_CLOCK_GETTIME */
    } else {
        struct timeval now;
        gettimeofday(&now, NULL);
        ticks = now.tv_sec;
        ticks *= LAKE_US_PER_SECOND;
        ticks += now.tv_usec;
    }
    return ticks;
}

u64 lake_rtc_frequency(void)
{
    if (lake_unlikely(!g_checked_monotonic))
        check_monotonic();

    if (lake_likely(g_has_monotonic)) {
#ifdef LAKE_HAS_CLOCK_GETTIME
        return LAKE_NS_PER_SECOND;
#elif defined(LAKE_PLATFORM_APPLE)
        u64 freq = mach_base_info.denom;
        freq *= LAKE_NS_PER_SECOND;
        freq /= mach_base_info.numer;
        return freq;
#endif /* LAKE_HAS_CLOCK_GETTIME */
    } 
    return LAKE_US_PER_SECOND;
}
#endif /* LAKE_PLATFORM_UNIX */
