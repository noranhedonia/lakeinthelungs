#include <lake/bedrock.h>

#if defined(LAKE_PLATFORM_WINDOWS)
#include <lake/private/windows.h>

void sys_thread_create(sys_thread_id *out_thread, void *(*procedure)(void *), void *argument)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0) {
        lake_fatal("Creating a joinable thread attribute with 'pthread_attr_setdetachstate' failed.");
        lake_debugtrap();
    }
    if (pthread_create((pthread_t *)out_thread, &attr, procedure, argument) != 0) {
        lake_fatal("Creating a thread with 'pthread_create' failed.");
        lake_debugtrap();
    }
    pthread_attr_destroy(&attr);
}

void sys_thread_destroy(sys_thread_id thread)
{
    lake_dbg_assert(!pthread_equal((pthread_t)thread, pthread_self()), LAKE_PANIC, "Thread can't destroy itself!");

    if (pthread_cancel((pthread_t)thread) != 0) {
        lake_error("Destroying a thread with 'pthread_cancel' failed.");
        return;
    }
    if (pthread_join((pthread_t)thread, NULL) != 0) {
        lake_error("Destroying a thread with 'pthread_join' failed.");
        return;
    }
}

void sys_thread_join(sys_thread_id thread)
{
    lake_dbg_assert(!pthread_equal((pthread_t)thread, pthread_self()), LAKE_PANIC, "Thread can't destroy itself!");

    if (pthread_join((pthread_t)thread, NULL) != 0) {
        lake_error("Joining a thread with 'pthread_join' (no cancel) failed.");
        return;
    }
}

void sys_thread_affinity(u32 thread_count, sys_thread_id const *threads, u32 cpu_count, u32 begin_cpu_idx)
{
    (void)thread_count;
    (void)threads;
    (void)cpu_count;
    (void)begin_cpu_index;
}
#endif /* LAKE_PLATFORM_WINDOWS */
