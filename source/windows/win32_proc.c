#include "../bedrock_internal.h"

#ifdef LAKE_PLATFORM_WINDOWS
#include <lake/bedrock/private/windows.h>

void sys_cpuinfo(s32 *out_threads, s32 *out_cores, s32 *out_packages)
{
    (void)out_threads;
    (void)out_cores;
    (void)out_packages;
    // TODO
}

void sys_meminfo(usize *out_total_ram, usize *out_page_size)
{
    (void)out_total_ram;
    (void)out_page_size;
    // TODO
}

void sys_hugetlbinfo(usize *out_hugepage_size, usize ceiling)
{
    (void)out_hugepage_size;
    (void)ceiling;
    // TODO
}

void sys_dump_stack_trace(void *stream)
{
    (void)stream;
    // TODO
}
#endif /* LAKE_PLATFORM_WINDOWS */
