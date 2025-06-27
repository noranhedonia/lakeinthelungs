#include <lake/bedrock/file_system.h>

#ifdef LAKE_PLATFORM_LINUX
lake_fs_observer *lake_fs_observer_assembly(lake_fs_observer_ref *ref)
{
    (void)ref;
    return nullptr;
}

bool lake_fs_observer_append(lake_fs_observer *observer, lake_fs_observer_ref *ref)
{
    (void)observer;
    (void)ref;
    return false;
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
