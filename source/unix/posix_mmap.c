#include "../internal.h"

#if defined(LAKE_PLATFORM_UNIX)
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h> /* strerror */

void *sys_mmap(usize page_aligned, usize hugepage_size)
{
    void *mapped = NULL;
    s32 flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE;

    if (!hugepage_size) {
        /* ignore */
#if defined(MAP_HUGE_4MB)
    } else if (hugepage_size == 4*1024*1024) {
        flags |= MAP_HUGE_4MB;
#endif /* 4MB */
#if defined(MAP_HUGE_2MB)
    } else if (hugepage_size == 2*1024*1024) {
        flags |= MAP_HUGE_2MB;
#endif /* 2MB */
#if defined(MAP_HUGE_1MB)
    } else if (hugepage_size == 1024*1024) {
        flags |= MAP_HUGE_1MB;
#endif /* 1MB */
    }
    mapped = mmap(NULL, page_aligned, PROT_NONE, flags, -1, 0);
    if (mapped == MAP_FAILED) {
        lake_log_from_critical_path(-4, "mmap failed to reserve virtual memory of %lu bytes (%lumb): %s.", 
                page_aligned, page_aligned >> 20, strerror(errno));
#ifndef LAKE_NDEBUG
        lake_debugtrap();
#endif /* LAKE_NDEBUG */
        return nullptr;
    }
    return mapped;
}

void sys_munmap(void *mapped, usize size_page_aligned)
{
    s32 res = munmap(mapped, size_page_aligned);
    if (res != 0) { lake_log_from_critical_path(-4, "Failed munmap with status %d.", res); }
}

bool sys_madvise(void *mapped, usize offset, usize size, bool commit_or_release)
{
    void       *raw_map = (void *)((sptr)mapped + offset);
    char const *errtype = commit_or_release ? "commitment" : "release";
    bool        success = 1;
#ifndef LAKE_NDEBUG
    if (size == 0 && "The page size must not be zero.") lake_debugtrap();
#endif /* LAKE_NDEBUG */

    /* commit physical memory */
    if (commit_or_release) {
        s32 res = mprotect(raw_map, size, PROT_READ | PROT_WRITE);
        if (res == 0)
            res = madvise(raw_map, size, MADV_WILLNEED);
        success = (res == 0);
    /* release physical memory */
    } else {
        s32 res = madvise(raw_map, size, MADV_DONTNEED);
        if (res == 0)
            res = mprotect(raw_map, size, PROT_NONE);
        success = (res == 0);
    }
    /* check for errors */
    if (!success) {
        lake_log_from_critical_path(2, "Failed %s of physical memory: %lu bytes (%lu MB) at %lu mapped offset (%lu MB).", errtype,
                size, size >> 20, offset, offset >> 20);
#ifdef LAKE_DEBUG
    } else {
        lake_log_from_critical_path(2, "Advise %s of physical memory: %lu bytes (%lu MB) at %lu mapped offset (%lu MB).", errtype,
                size, size >> 20, offset, offset >> 20);
#endif /* LAKE_DEBUG */
    }
    return success;
}
#endif /* LAKE_PLATFORM_UNIX */
