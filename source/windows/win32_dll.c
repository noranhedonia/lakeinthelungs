#include <lake/bedrock/bedrock.h>

#if defined(LAKE_PLATFORM_WINDOWS)
#include <lake/bedrock/private/windows.h>

void *lake_open_library(char const *libname)
{
    (void)libname;
    return nullptr;
}

void lake_close_library(void *library)
{
    (void)library;
}

void *lake_get_proc_address(void *library, char const *procname)
{
    (void)library;
    (void)procname;
    return nullptr;
}
#endif /* LAKE_PLATFORM_WINDOWS */
