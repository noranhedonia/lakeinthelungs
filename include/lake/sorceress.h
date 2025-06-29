#pragma once
#ifndef LAKE_SORCERESS
#error "Don't include this header unless the Sorceress framework was built in."
#endif

/** @file lake/sorceress.h
 *  @brief The game engine.
 *
 *  TODO docs
 */
#include <lake/inthelungs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** The video game interface implementation. */
LAKE_DECL_INTERFACE(sorceress);

/** Flags to control the encore. */
typedef u32 sorceress_control_flags;
typedef enum sorceress_control_flag_bits : sorceress_control_flags {
    /** Immediately will wait for all current work to finish, and exit the application. */
    sorceress_control_flag_exit_app            = (1u << 0),
    /** We'll exit from the gameloop, safely commit any deferred engine updates, and continue normally. */
    sorceress_control_flag_compose_movements   = (1u << 1),
} sorceress_control_flag_bits;
#define sorceress_control_flag_exit_or_compose    \
    (sorceress_control_flag_exit_app | sorceress_control_flag_compose_movements)

/** TODO A complete description of the engine state, is assembled from an encore composer. */
typedef struct sorceress_composition {
    /* TODO */
    PFN_lake_interface_impl                 sorceress_interface_impl;
} sorceress_composition;

/** TODO directed acyclic graph of in-engine state dependencies between transition handlers.
 *  Each node is a transition, and edges are dependencies between these transitions. So each 
 *  transition defines what it requires, and what it provides. Transitions that have no 
 *  dependencies between them can run in parallel, and by design will. */
typedef struct sorceress_encore_composer {
    sorceress_composition                  *composition;
    lake_dagraph                            dagraph;
} sorceress_encore_composer;

/** Holds all state of the prototype. From inside the gameloop, this state is read-only. */
typedef struct sorceress_encore {
    /** App metadata and host details acquired from the framework. */
    lake_bedrock const                     *bedrock;
    /** Current composition of the engine state. When collecting movements via the encore_composer, 
     *  the composed piece is compared with the live one to figure out what and how should things change. */
    sorceress_composition                   live_composition;
    /** An interface that is to be implemented by a video game application. */
    sorceress_interface                     sorceress;
    /** Used to access the display backend. The primary viewport is always active during the gameloop. */
    lake_video                              video;
    /** Encapsulates rendering work for parallel and mGPU setups. */
    lake_renderer                           renderer;
    /** Interface of the rendering backend. */
    moon_interface                          moon;
    /** The primary rendering device. TODO Later shift into a mGPU design when I'll 
     *  introduce render graphs into the engine. For consistency, the primary device 
     *  is the only one allowed to create and use swapchains for the Hadal windows. */
    moon_device                             device;
    /** Swapchain connected to the primary window used for presentation. */
    moon_swapchain                          swapchain;
} sorceress_encore;

/** Work fed forward during the pipeline stages, only one stage at a time can access unique instance of work. */
typedef struct sorceress_pipeline_work {
    /** Incrementally rising counter to indicate the current frame (and CPU timeline). */
    u64                                     timeline;
    /** The delta time, whatever it is for the current frame. It doesn't reflect the latency well, 
     *  dt is the difference between two consecutive frames entering the pipeline, not leaving it. */
    f64                                     dt;
    /** Constant hints acquired from the bedrock framework, for convenient access. */
    u32                                     frames_in_flight, worker_thread_count;
    /** Some of the flags are checked during resolve to control the encore. */
    sorceress_control_flags                 control;
    /** For now we use only one camera view for the render.
     *  The camera's transform is set during the gameplay stage. */
    lake_camera                             camera;
    /** Use this arena to feed forward some stuff, such as handles to 
     *  swapchains, semaphores, arrays of sorted indices, etc. */
    lake_arena_allocator                    arena;
    /** One for every rendering device. Describes complete per-device GPUexec workload, 
     *  per-queue submits can be done in parallel without blocking. Accessed by device index. */
    lake_render_submit                     *submits;
    /** Presentation is a responsibility of the primary device, as it's the only device allowed 
     *  to create swapchains. The swapchains array should refer to the swapchains defined in
     *  video, unless a lost surface makes the swapchain obsolete for presentation. */
    moon_device_present                     present;
    /** A composer to be accessed in parallel, is used to defer changes to the encore state. */
    struct sorceress_encore_composer       *encore_composer;
    /** Read-only access to the engine state. Changes should be done via transitions. */
    struct sorceress_encore const          *encore;
    /** Read-only access to the previously processed work. */
    struct sorceress_pipeline_work const   *last_work;
    /** Unsafe access to the next work to enter this pipeline stage. */
    struct sorceress_pipeline_work         *next_work;
} sorceress_pipeline_work;

/** It's the entry point to the engine. When this function returns, Bedrock will shutdown
 *  all internal systems and release any physical resources that are still in use.
 *  At this point already we can be sure that we run inside a fiber. */
LAKEAPI FN_LAKE_BEDROCK_MAIN(sorceress_bedrock_main, sorceress_composition *composition);

/** Composes the encore movements by parsing an argument list. This can be called outside of Bedrock. */
LAKEAPI LAKE_NONNULL_ALL
s32 LAKECALL sorceress_compose_w_args(
    sorceress_composition *out_composition, 
    s32                    argc, 
    char const           **argv);

/** The gameplay stage. It is responsible for simulating the game world, processing user and system events,
 *  handling scene transitions and prepares data that is to be rendered in later stages. This stage may also
 *  defer computationally heavy work (e.g. animation skinning, specular audio, physics simulation) into GPU 
 *  execution by feeding this data further into the pipeline. */
FN_LAKE_WORK(sorceress_pipeline_gameplay, sorceress_pipeline_work *work);

/** The rendering stage. Processes the auditory and rendering logic for the current frame and associated 
 *  swapchain images. Moves any staging resources into VRAM and records command buffers to later execute 
 *  that work on the GPU(s). GPU execution is deferred, as this stage is not allowed to submit GPU commands. */
FN_LAKE_WORK(sorceress_pipeline_rendering, sorceress_pipeline_work *work);

/** The GPU execution stage. Submits all work and data collected from previous stages into available GPUs.
 *  This stage is responsible for GPU-GPU and CPU-GPU synchronization. Results are presented via swapchain.
 *  As it's the last stage, leaving it means completing the pipeline. */
FN_LAKE_WORK(sorceress_pipeline_gpuexec, sorceress_pipeline_work *work);

/* TODO DO THE THING MAAAAAAAAAAAAAN */
typedef void (LAKECALL *PFN_sorceress_movement)(struct sorceress_encore_composer *encore_composer);
#define FN_SORCERESS_MOVEMENT(backend) \
    void LAKECALL _sorceress_##backend##_movement(struct sorceress_encore_composer *encore_composer)

/** This interface must be implemented by a video game application. */
struct sorceress_interface_impl {
    lake_interface_header   header;
    PFN_sorceress_movement  movement;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
