#include <lake/bedrock/file_system.h>

#ifdef LAKE_PLATFORM_LINUX
lake_fs_observer *lake_fs_observer_assembly(lake_fs_observer_ref *ref)
{
    (void)ref;
    return nullptr;
}

s32 lake_fs_observer_append_ref(lake_fs_observer *observer, lake_fs_observer_ref *ref)
{
    (void)observer;
    (void)ref;
    return -1;
}

void lake_fs_observer_remove_ref(lake_fs_observer *observer, s32 context)
{
    (void)observer;
    (void)context;
}

FN_LAKE_WORK(lake_fs_observer_destructor, lake_fs_observer *observer)
{
    (void)observer;
}

char *lake_fs_executable_path(void)
{
    return nullptr;
}
#endif /* LAKE_PLATFORM_LINUX */
