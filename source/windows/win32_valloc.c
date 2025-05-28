#include <lake/bedrock/bedrock.h>

#if defined(LAKE_PLATFORM_WINDOWS)
#include <lake/bedrock/private/windows.h>

void *sys_mmap(usize page_aligned, usize hugepage_size)
{
    (void)page_aligned;
    (void)hugepage_size;
    return nullptr;
}

void sys_munmap(void *mapped, usize size_page_aligned)
{
    (void)mapped;
    (void)size_page_aligned;
}

bool sys_madvise(void *mapped, usize offset, usize size, enum sys_madvise_mode mode)
{
    (void)mapped;
    (void)offset;
    (void)size;
    (void)mode;
}
#endif /* LAKE_PLATFORM_WINDOWS */
