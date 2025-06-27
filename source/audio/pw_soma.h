#pragma once

#include <lake/modules/soma.h>
#ifdef SOMA_PIPEWIRE

#include <pipewire/pipewire.h>
#include <pipewire/loop.h>
#include <pipewire/extensions/metadata.h>
#include <spa/param/audio/format-utils.h>
#include <spa/utils/json.h>

#ifndef PW_KEY_CONFIG_NAME
#define PW_KEY_CONFIG_NAME "config.name"
#endif
#ifndef PW_KEY_NODE_RATE
#define PW_KEY_NODE_RATE "node.rate"
#endif
#ifndef PW_KEY_TARGET_OBJECT
#define PW_KEY_TARGET_OBJECT "target.object"
#endif

typedef char const *(*PFN_pw_get_library_version)(void);
typedef void (*PFN_pw_init)(s32 *, char ***);
typedef void (*PFN_pw_deinit)(void);
typedef struct pw_loop *(*PFN_pw_loop_new)(struct spa_dict const *props);
typedef void (*PFN_pw_loop_destroy)(struct pw_loop *loop);
typedef s32 (*PFN_pw_loop_set_name)(struct pw_loop *loop, char const *name);
typedef struct pw_context *(*PFN_pw_context_new)(struct pw_loop *, struct pw_properties *, usize);
typedef void (*PFN_pw_context_destroy)(struct pw_context *);
typedef struct pw_core *(*PFN_pw_context_connect)(struct pw_context *, struct pw_properties *, usize);
typedef void (*PFN_pw_proxy_add_object_listener)(struct pw_proxy *, struct spa_hook *, void const *, void *);
typedef void *(*PFN_pw_proxy_get_user_data)(struct pw_proxy *);
typedef void (*PFN_pw_proxy_destroy)(struct pw_proxy *);
typedef s32 (*PFN_pw_core_disconnect)(struct pw_core *);
typedef struct pw_stream *(*PFN_pw_stream_new_simple)(struct pw_loop *, const char *, struct pw_properties *, struct pw_stream_events const *, void *);
typedef void (*PFN_pw_stream_destroy)(struct pw_stream *);
typedef s32 (*PFN_pw_stream_connect)(struct pw_stream *, enum pw_direction, u32, enum pw_stream_flags, struct spa_pod const **, u32);
typedef enum pw_stream_state (*PFN_pw_stream_get_state)(struct pw_stream *, char const **);
typedef struct pw_buffer *(*PFN_pw_stream_dequeue_buffer)(struct pw_stream *);
typedef s32 (*PFN_pw_stream_queue_buffer)(struct pw_stream *, struct pw_buffer *);
typedef struct pw_properties *(*PFN_pw_properties_new)(char const *, ...) SPA_SENTINEL;
typedef s32 (*PFN_pw_properties_set)(struct pw_properties *, char const *, char const *);
typedef s32 (*PFN_pw_properties_setf)(struct pw_properties *, char const *, char const *, ...) SPA_PRINTF_FUNC(3, 4);

#define PW_MIN_SAMPLES                  32 /* about 0.67ms at 48kHz */
#define PW_BASE_CLOCK_RATE              48000

#define PW_POD_BUFFER_LENGTH            1024
#define PW_THREAD_NAME_BUFFER_LENGTH    128
#define PW_MAX_IDENTIFIER_LENGTH        256

enum PW_READY_FLAGS {
    PW_READY_FLAG_BUFFER_ADDED = 0x1,
    PW_READY_FLAG_STREAM_READY = 0x2,
    PW_READY_FLAG_ALL_BITS = 0x3,
};

#define PW_ID_TO_HANDLE(x) (void *)((uptr)x)
#define PW_HANDLE_TO_ID(x) (u32)((uptr)x)

struct soma_impl {
    struct soma_interface_impl          interface;

    void                               *pipewire_library;
    PFN_pw_get_library_version          pw_get_library_version;
    PFN_pw_init                         pw_init;
    PFN_pw_deinit                       pw_deinit;
    PFN_pw_loop_new                     pw_loop_new;           
    PFN_pw_loop_destroy                 pw_loop_destroy;
    PFN_pw_loop_set_name                pw_loop_set_name;
    PFN_pw_context_new                  pw_context_new;
    PFN_pw_context_destroy              pw_context_destroy;
    PFN_pw_context_connect              pw_context_connect;
    PFN_pw_proxy_add_object_listener    pw_proxy_add_object_listener;
    PFN_pw_proxy_get_user_data          pw_proxy_get_user_data;
    PFN_pw_proxy_destroy                pw_proxy_destroy;
    PFN_pw_core_disconnect              pw_core_disconnect;
    PFN_pw_stream_new_simple            pw_stream_new_simple;
    PFN_pw_stream_destroy               pw_stream_destroy;
    PFN_pw_stream_connect               pw_stream_connect;
    PFN_pw_stream_get_state             pw_stream_get_state;
    PFN_pw_stream_dequeue_buffer        pw_stream_dequeue_buffer;
    PFN_pw_stream_queue_buffer          pw_stream_queue_buffer;
    PFN_pw_properties_new               pw_properties_new;
    PFN_pw_properties_set               pw_properties_set;
    PFN_pw_properties_setf              pw_properties_setf;

    s32                                 version_major;
    s32                                 version_minor;
    s32                                 version_patch;
};
#endif /* SOMA_PIPEWIRE */
