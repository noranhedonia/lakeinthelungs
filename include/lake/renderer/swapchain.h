#pragma once

/** @file lake/renderer/swapchain.h
 *  @brief TODO
 *
 *  TODO docs
 */
#include <lake/renderer/render_resources.h>
#include <lake/renderer/device.h>

struct hadal_impl;
struct hadal_window_impl;

typedef enum moon_present_mode : s8 {
    moon_present_mode_immediate = 0,
    moon_present_mode_mailbox,
    moon_present_mode_fifo,
    moon_present_mode_fifo_relaxed,
} moon_present_mode;

typedef u16 moon_present_transform;
typedef enum moon_present_transform_bits : moon_present_transform {
    moon_present_transform_identity             = (1u << 0),
    moon_present_transform_rotate_90            = (1u << 1),
    moon_present_transform_rotate_180           = (1u << 2),
    moon_present_transform_rotate_270           = (1u << 3),
    moon_present_transform_mirror               = (1u << 4),
    moon_present_transform_mirror_rotate_90     = (1u << 5),
    moon_present_transform_mirror_rotate_180    = (1u << 6),
    moon_present_transform_mirror_rotate_270    = (1u << 7),
    moon_present_transform_inherit              = (1u << 8),
} moon_present_transform_bits;

/** A custom procedure to select the surface format from an array of supported formats.
 *  Should return an index into the `formats` array. */
typedef s32 (LAKECALL *PFN_moon_surface_format_selector)(u32 format_count, moon_format const *formats);
#define FN_MOON_SURFACE_FORMAT_SELECTOR(fn) \
    s32 LAKECALL fn(u32 format_count, moon_format const *formats)

/** Details needed to create a swapchain. */
typedef struct moon_swapchain_assembly {
    /** A window obtained from the display backend. */
    struct hadal_window_impl           *native_window;
    /** Custom selector for the surface format. If NULL, a default selector will be provided. */
    PFN_moon_surface_format_selector    surface_format_selector;
    /** Indices of timeline semaphores for presentation will be cycled by this value. */
    s16                                 max_allowed_frames_in_flight;
    /** Describes how the swapchain image will be transformed at presentation. */
    moon_present_transform              present_transform;
    /** The preferred present mode will be selected if possible. */
    moon_present_mode                   present_mode;
    /** This queue type will be selected for presentation, fallback is the main queue. */
    moon_queue_type                     queue_type;
    /** Usage bits for swapchain images. */
    moon_texture_usage                  image_usage;
    /** The format in use for swapchain images. */
    moon_format                         image_format;
    /** Name of the swapchain for debugging. */
    lake_small_string                   name;
} moon_swapchain_assembly;
static constexpr moon_swapchain_assembly MOON_SWAPCHAIN_ASSEMBLY_INIT = {
    .native_window = nullptr,
    .surface_format_selector = nullptr,
    .max_allowed_frames_in_flight = INT16_MAX,
    .present_transform = moon_present_transform_identity,
    .present_mode = moon_present_mode_mailbox,
    .image_usage = moon_texture_usage_color_attachment,
    .queue_type = moon_queue_type_main,
};

/** Assemble a swapchain. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_swapchain_assembly)(struct moon_device_impl *device, moon_swapchain_assembly const *assembly, struct moon_swapchain_impl **out_swapchain);
#define FN_MOON_SWAPCHAIN_ASSEMBLY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_swapchain_assembly(struct moon_device_impl *device, moon_swapchain_assembly const *assembly, struct moon_swapchain_impl **out_swapchain)

/** Destroy a swapchain. */
PFN_LAKE_WORK(PFN_moon_swapchain_zero_refcnt, struct moon_swapchain_impl *swapchain);
#define FN_MOON_SWAPCHAIN_ZERO_REFCNT(backend) \
    FN_LAKE_WORK(_moon_##backend##_swapchain_zero_refcnt, struct moon_swapchain_impl *swapchain)

/** Limits frames in flight. Blocks until GPU catches up to the max number of frames in flight.
 *  DOES NOT WAIT for the swapchain image to be available, one must STILL use the acquire semaphore.
 *  This function DOES WAIT until there is a frame in flight available to prepare on the CPU. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_swapchain_wait_for_next_frame)(struct moon_swapchain_impl *swapchain);
#define FN_MOON_SWAPCHAIN_WAIT_FOR_NEXT_FRAME(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_swapchain_wait_for_next_frame(struct moon_swapchain_impl *swapchain)

/** The texture handle may change between calls. This must be called to obtain a new swapchain image 
 *  to be used for rendering. WARNING: texture handles returned from the swapchain are INVALID after 
 *  the swapchain is destroyed, or either resize or set_present_mode is called. This function may 
 *  internally call into `PFN_moon_swapchain_wait_for_next_frame`. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_swapchain_acquire_next_image)(struct moon_swapchain_impl *swapchain, moon_texture_id *out_texture);
#define FN_MOON_SWAPCHAIN_ACQUIRE_NEXT_IMAGE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_swapchain_acquire_next_image(struct moon_swapchain_impl *swapchain, moon_texture_id *out_texture)

/** The acquire semaphore must be waited on in the first submission that uses the last acquired image. 
 *  This semaphore may change between acquires, so it needs to be re-queried after every current_acquire_semaphore call. */
typedef LAKE_NODISCARD struct moon_binary_semaphore_impl *(LAKECALL *PFN_moon_swapchain_current_acquire_semaphore)(struct moon_swapchain_impl *swapchain);
#define FN_MOON_SWAPCHAIN_CURRENT_ACQUIRE_SEMAPHORE(backend) \
    LAKE_NODISCARD struct moon_binary_semaphore_impl *LAKECALL _moon_##backend##_swapchain_current_acquire_semaphore(struct moon_swapchain_impl *swapchain)

/** The present semaphore must be signaled in the last submission that uses the last acquired swapchain image.
 *  The present semaphore must be waited on in the present of the last acquired image. This semaphore may 
 *  change between acquires, so it needs to be re-queried after every current_acquire_semaphore call. */
typedef LAKE_NODISCARD struct moon_binary_semaphore_impl *(LAKECALL *PFN_moon_swapchain_current_present_semaphore)(struct moon_swapchain_impl *swapchain);
#define FN_MOON_SWAPCHAIN_CURRENT_PRESENT_SEMAPHORE(backend) \
    LAKE_NODISCARD struct moon_binary_semaphore_impl *LAKECALL _moon_##backend##_swapchain_current_present_semaphore(struct moon_swapchain_impl *swapchain)

/** The last submission that uses the swapchain image needs to signal the timeline with the CPU value. */
typedef LAKE_NODISCARD u64 (LAKECALL *PFN_moon_swapchain_current_cpu_timeline_value)(struct moon_swapchain_impl *swapchain);
#define FN_MOON_SWAPCHAIN_CURRENT_CPU_TIMELINE_VALUE(backend) \
    LAKE_NODISCARD u64 LAKECALL _moon_##backend##_swapchain_current_cpu_timeline_value(struct moon_swapchain_impl *swapchain)

/** The swapchain needs to know when the last use of the swapchain happens to limit the frames in flight. In the last 
 *  submission that uses the swapchain image, signal this timeline semaphore with the CPU timeline value. The CPU value 
 *  timeline is incremented whenever acquire is called. The GPU timeline must be manually incremented by the user via 
 *  a submit. The difference between CPU and GPU timeline describes how many frames in flight the GPU is behind the CPU. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_swapchain_current_timeline_pair)(struct moon_swapchain_impl *swapchain, moon_timeline_pair *out_timeline);
#define FN_MOON_SWAPCHAIN_CURRENT_TIMELINE_PAIR(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_swapchain_current_timeline_pair(struct moon_swapchain_impl *swapchain, moon_timeline_pair *out_timeline)

/** The swapchain needs to know when the last use of the swapchain happens to limit the frames in flight.
 *  In the last submission that uses the swapchain image, signal this timeline semaphore with the CPU timeline value. */
typedef LAKE_NODISCARD struct moon_timeline_semaphore_impl *(LAKECALL *PFN_moon_swapchain_gpu_timeline_semaphore)(struct moon_swapchain_impl *swapchain);
#define FN_MOON_SWAPCHAIN_GPU_TIMELINE_SEMAPHORE(backend) \
    LAKE_NODISCARD struct moon_timeline_semaphore_impl *LAKECALL _moon_##backend##_swapchain_gpu_timeline_semaphore(struct moon_swapchain_impl *swapchain)

/** Recreates the swapchain with new present mode. Due to wsi limitations this function will WAIT IDLE the device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_swapchain_set_present_mode)(struct moon_swapchain_impl *swapchain, moon_present_mode present_mode);
#define FN_MOON_SWAPCHAIN_SET_PRESENT_MODE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_swapchain_set_present_mode(struct moon_swapchain_impl *swapchain, moon_present_mode present_mode)

/** When the window size changes the swapchain is in an invalid state for new commands.
 *  Calling resize will recreate the swapchain using the updated window size. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_swapchain_resize)(struct moon_swapchain_impl *swapchain);
#define FN_MOON_SWAPCHAIN_RESIZE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_swapchain_resize(struct moon_swapchain_impl *swapchain)

/** Calls into the display backend to setup windowing support, necessary to use the swapchain. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_connect_to_display)(struct moon_impl *moon, struct hadal_impl *hadal);
#define FN_MOON_CONNECT_TO_DISPLAY(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_connect_to_display(struct moon_impl *moon, struct hadal_impl *hadal)
