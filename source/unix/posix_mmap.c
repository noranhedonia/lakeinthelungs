#include "../bedrock_internal.h"

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
        lake_fatal("mmap failed to reserve virtual memory of %lu bytes (%lumb): %s.", 
                page_aligned, page_aligned >> 20, strerror(errno));
        return nullptr;
    }
    return mapped;
}

void sys_munmap(void *mapped, usize size_page_aligned)
{
    s32 res = munmap(mapped, size_page_aligned);
    lake_dbg_assert(!res, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);
}

bool sys_madvise(void *mapped, usize offset, usize size, enum sys_madvise_mode mode)
{
    void       *raw_map = (void *)((sptr)mapped + offset);
    char const *errtype = mode == sys_madvise_mode_commit ? "commitment" : "release";
    bool        success = 1;
    lake_dbg_assert(size > 0, LAKE_INVALID_PARAMETERS, "The page size must not be zero.");

    /* commit physical memory */
    if (mode == sys_madvise_mode_commit) {
        s32 res = mprotect(raw_map, size, PROT_READ | PROT_WRITE);
        if (res == 0)
            res = madvise(raw_map, size, MADV_WILLNEED);
        success = (res == 0);
    /* release physical memory */
    } else if (mode == sys_madvise_mode_release) {
        s32 res = madvise(raw_map, size, MADV_DONTNEED);
        if (res == 0)
            res = mprotect(raw_map, size, PROT_NONE);
        success = (res == 0);
    }
    /* check for errors */
    if (!success) {
        lake_error("Failed %s of physical memory: %lu bytes (%lu MB) at %lu mapped offset (%lu MB).", errtype,
                size, size >> 20, offset, offset >> 20);
#ifdef LAKE_DEBUG
    } else {
        lake_dbg_1("Advise %s of physical memory: %lu bytes (%lu MB) at %lu mapped offset (%lu MB).", errtype,
                size, size >> 20, offset, offset >> 20);
#endif /* LAKE_DEBUG */
    }
    return success;
}
#endif /* LAKE_PLATFORM_UNIX */
