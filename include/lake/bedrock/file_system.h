#pragma once

/** @file lake/bedrock/file_system.h
 *  @brief TODO docs
 */
#include <lake/bedrock/drifter.h>
#include <lake/bedrock/job_system.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum lake_fs_observer_event : s8 {
    lake_fs_observer_event_unspecified = 0,
    lake_fs_observer_event_created,
    lake_fs_observer_event_deleted,
    lake_fs_observer_event_modified,
    lake_fs_observer_event_renamed,
    lake_fs_observer_event_changed_attributes,
    lake_fs_observer_event_opened,
} lake_fs_observer_event;

/** This function is called on a file observer event. */
typedef void (LAKECALL *PFN_lake_fs_observer_work)(
        void                   *userdata,
        lake_fs_observer_event  event, 
        char const             *path, 
        s32                     ctx);
#define FN_LAKE_FS_OBSERVER_WORK(fn) \
    void LAKECALL fn(void *userdata, lake_fs_observer_event event, char const *path, s32 ctx)

/** Information passed when creating an observer for the filesystem. */
typedef struct lake_fs_observer_ref {
    char                       *path;       /**< Path to the file or directory to be observed, it should exist or will fail. */
    PFN_lake_fs_observer_work   work;       /**< This function will be called whenever the file/directory changes. */
    s32                         context;    /**< Optional context that can be given to the callback if needed. */
    void                       *userdata;   /**< Optional data given to the callback if needed. */
} lake_fs_observer_ref;

/** An opaque handle for the file observer. */
typedef struct lake_fs_observer lake_fs_observer;

/** Creates a file observer with a first reference. The reference can be NULL,
 *  then the observer will be empty. This requires a call into the file system. */
LAKEAPI lake_fs_observer *LAKECALL lake_fs_observer_assembly(lake_fs_observer_ref *ref);

/** Appends the observer with a given file or directory reference. */
LAKEAPI LAKE_NONNULL_ALL
bool LAKECALL lake_fs_observer_append(lake_fs_observer *observer, lake_fs_observer_ref *ref);

/** Destroys an observers instance in the file system. */
LAKEAPI LAKE_NONNULL_ALL
FN_LAKE_WORK(lake_fs_observer_destructor, lake_fs_observer *observer);

/** A string of the executable path allocated using drifter. */
LAKEAPI LAKE_NONNULL_ALL
char *LAKECALL lake_fs_executable_path(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
