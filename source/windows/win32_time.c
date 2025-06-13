#include "../internal.h"

#ifdef LAKE_PLATFORM_WINDOWS
#include <lake/private/windows.h>

u64 lake_rtc_counter(void)
{
    return 0;
}

u64 lake_rtc_frequency(void)
{
    return 0;
}
#endif /* LAKE_PLATFORM_WINDOWS */
