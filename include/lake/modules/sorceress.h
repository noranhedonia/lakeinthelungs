#pragma once

/** @file lake/modules/sorceress.h
 *  @brief Application framework.
 *
 *  TODO docs, sorceress
 */
#include <lake/bedrock/bedrock.h>
#include <lake/data_structures/dagraph.h>
#include <lake/data_structures/darray.h>
#include <lake/math/crypto.h>

#ifdef LAKE_SORCERESS
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** The video game interface implementation. */
LAKE_DECL_INTERFACE(sorceress);
/** A term of work within the gameloop pipeline. */
LAKE_DECL_HANDLE_INTERFACED(sorceress_work, sorceress_interface *sorceress);

/** Flags to control movements. */
typedef u64 sorceress_control_flags;
typedef enum sorceress_control_flag_bits : sorceress_control_flags {
    /** Immediately will wait for all current work to finish, and exit the application. */
    sorceress_control_flag_should_exit      = (1llu << 0),
    /** We'll exit from the gameloop, safely commit any deferred engine updates, and continue normally. */
    sorceress_control_flag_should_compose   = (1llu << 1),
} sorceress_control_flag_bits;
#define sorceress_control_flag_should_exit_or_compose    \
    (sorceress_control_flag_should_exit | sorceress_control_flag_should_compose)

/** TODO update the game state. In the future I'd like to make this system a declarative way to assemble the engine state,
 *  and to allow engine state transitions between a live and a staging composition - and optimize for these transitions to 
 *  happen in parallel. It seems it will be a lot of work, and I have no real solution figured out, thus I'll defer that
 *  idea and focus on actually implementing stuff to make an interactive demo. XXX */
typedef void (LAKECALL *PFN_sorceress_invoke_movements)(struct sorceress_impl *sorceress, sorceress_control_flags *control);
#define FN_SORCERESS_INVOKE_MOVEMENTS(VAR) \
    void LAKECALL _sorceress_##VAR##_invoke_movements(struct sorceress_impl *sorceress, sorceress_control_flags *control)

/** Acquires an array of work that will be passed into gameloop pipeline stages. The application 
 *  may decide how many work it wants to allocate. The output count MUST BE a power of two, and must 
 *  be no less than the pipeline stages running (min: 4). The work array CAN be allocated via Drifter! :D
 *  @return Stride of the work element - sizeof(struct sorceress_work_impl). */
typedef usize (LAKECALL *PFN_sorceress_acquire_work)(struct sorceress_impl *sorceress, u32 *out_work_min4_pow2_count, struct sorceress_work_impl **out_work);
#define FN_SORCERESS_ACQUIRE_WORK(VAR) \
    usize LAKECALL _sorceress_##VAR##_acquire_work(struct sorceress_impl *sorceress, u32 *out_work_count, struct sorceress_work_impl **out_work)

/** Releases the work array. It's called when Sorceress leaves the gameloop, any cleanup can be done here. */
typedef void (LAKECALL *PFN_sorceress_release_work)(u32 work_count, struct sorceress_work_impl *work);
#define FN_SORCERESS_RELEASE_WORK(VAR) \
    void LAKECALL _sorceress_##VAR##_release_work(u32 work_count, struct sorceress_work_impl *work)

/** Called before work enters the pipeline. Can be used to configure settings that should be immutable
 *  during an iteration of this work, read window events, and so on - figure out if and what work to do.
 *  The header structure is already set before calling this function. */
typedef sorceress_control_flags (LAKECALL *PFN_sorceress_begin_of_pipe)(struct sorceress_work_impl *work);
#define FN_SORCERESS_BEGIN_OF_PIPE(VAR) \
    sorceress_control_flags LAKECALL _sorceress_##VAR##_begin_of_pipe(struct sorceress_work_impl *work)

/** Called after the work leaves the gameloop pipeline, used to cleanup or validate results. */
typedef sorceress_control_flags (LAKECALL *PFN_sorceress_end_of_pipe)(struct sorceress_work_impl *work);
#define FN_SORCERESS_END_OF_PIPE(VAR) \
    sorceress_control_flags LAKECALL _sorceress_##VAR##_end_of_pipe(struct sorceress_work_impl *work)

/** The gameplay stage. It is responsible for simulating the game world, processing user and system events,
 *  handling scene transitions and prepares data that is to be rendered in later stages. This stage may also
 *  defer computationally heavy work (e.g. animation skinning, specular audio, physics simulation) into GPU 
 *  execution by feeding this data further into the pipeline. */
PFN_LAKE_WORK(PFN_sorceress_stage_gameplay, struct sorceress_work_impl *work);
#define FN_SORCERESS_STAGE_GAMEPLAY(VAR) \
    FN_LAKE_WORK(_sorceress_##VAR##_stage_gameplay, struct sorceress_work_impl *work)

/** The rendering stage. Processes the auditory and rendering logic for the current frame and associated 
 *  swapchain images. Moves any staging resources into VRAM and records command buffers to later execute 
 *  that work on the GPU(s). GPU execution is deferred, as this stage is not allowed to submit GPU commands. */
PFN_LAKE_WORK(PFN_sorceress_stage_rendering, struct sorceress_work_impl *work);
#define FN_SORCERESS_STAGE_RENDERING(VAR) \
    FN_LAKE_WORK(_sorceress_##VAR##_stage_rendering, struct sorceress_work_impl *work)

/** The GPU execution stage. Submits all work and data collected from previous stages into available GPUs.
 *  This stage is responsible for GPU-GPU and CPU-GPU synchronization. Results are presented via swapchain.
 *  As it's the last stage, leaving it means completing the pipeline. */
PFN_LAKE_WORK(PFN_sorceress_stage_gpuexec, struct sorceress_work_impl *work);
#define FN_SORCERESS_STAGE_GPUEXEC(VAR) \
    FN_LAKE_WORK(_sorceress_##VAR##_stage_gpuexec, struct sorceress_work_impl *work)

/** This interface must be implemented by a video game application. */
struct sorceress_interface_impl {
    lake_interface_header               header;
    PFN_sorceress_invoke_movements      invoke_movements;
    PFN_sorceress_acquire_work          acquire_work;
    PFN_sorceress_release_work          release_work;
    PFN_sorceress_begin_of_pipe         begin_of_pipe;
    PFN_sorceress_end_of_pipe           end_of_pipe;
    PFN_sorceress_stage_gameplay        stage_gameplay;
    PFN_sorceress_stage_rendering       stage_rendering;
    PFN_sorceress_stage_gpuexec         stage_gpuexec;
};

/** Header for `sorceress_work_impl`, can be safely cast into that structure. */
typedef struct LAKE_ALIGNMENT(64) sorceress_work_header {
    sorceress_interface                 sorceress;
    /** Unsafe access to the next work to enter this pipeline stage. */
    struct sorceress_work_impl         *next_work;
    /** Read-only access to the previously processed work. */
    struct sorceress_work_impl const   *last_work;
    /** Some of the flags are checked during resolve to control the encore. */
    sorceress_control_flags             control;
    /** Incrementally rising counter to indicate the current frame (and CPU timeline). */
    u64                                 timeline;
    /** The delta time, whatever it is for the current frame. It doesn't reflect the latency well, 
     *  dt is the difference between two consecutive frames entering the pipeline, not leaving it. */
    f64                                 dt;
    /** XXX reserved. */
    u8                              pad1[16];
} sorceress_work_header;

/** It's the entry point to the framework. */
LAKEAPI FN_LAKE_BEDROCK_MAIN(sorceress_bedrock_main, PFN_lake_interface_impl impl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LAKE_SORCERESS */
