#pragma once

#include <lake/moon.h>
#include <lake/data_structures/darray.h>
#include <lake/data_structures/deque.h>
#ifdef MOON_VULKAN

FN_MOON_LIST_DEVICE_DETAILS(vulkan);
FN_MOON_DEVICE_ASSEMBLY(vulkan);
FN_MOON_DEVICE_DESTRUCTOR(vulkan);
FN_MOON_DEVICE_QUEUE_COUNT(vulkan);
FN_MOON_DEVICE_QUEUE_WAIT_IDLE(vulkan);
FN_MOON_DEVICE_WAIT_IDLE(vulkan);
FN_MOON_DEVICE_SUBMIT_COMMANDS(vulkan);
FN_MOON_DEVICE_PRESENT_FRAMES(vulkan);
FN_MOON_DEVICE_DESTROY_DEFERRED(vulkan);
FN_MOON_DEVICE_HEAP_ASSEMBLY(vulkan);
FN_MOON_DEVICE_HEAP_DESTRUCTOR(vulkan);
FN_MOON_DEVICE_BUFFER_MEMORY_REQUIREMENTS(vulkan);
FN_MOON_DEVICE_TEXTURE_MEMORY_REQUIREMENTS(vulkan);
FN_MOON_DEVICE_MEMORY_REPORT(vulkan);

FN_MOON_CREATE_BUFFER(vulkan);
FN_MOON_CREATE_BUFFER_FROM_HEAP(vulkan);
FN_MOON_CREATE_TEXTURE(vulkan);
FN_MOON_CREATE_TEXTURE_FROM_HEAP(vulkan);
FN_MOON_CREATE_TEXTURE_VIEW(vulkan);
FN_MOON_CREATE_SAMPLER(vulkan);
FN_MOON_CREATE_TLAS(vulkan);
FN_MOON_CREATE_TLAS_FROM_BUFFER(vulkan);
FN_MOON_CREATE_BLAS(vulkan);
FN_MOON_CREATE_BLAS_FROM_BUFFER(vulkan);

FN_MOON_IS_BUFFER_VALID(vulkan);
FN_MOON_IS_TEXTURE_VALID(vulkan);
FN_MOON_IS_TEXTURE_VIEW_VALID(vulkan);
FN_MOON_IS_SAMPLER_VALID(vulkan);
FN_MOON_IS_TLAS_VALID(vulkan);
FN_MOON_IS_BLAS_VALID(vulkan);

FN_MOON_BUFFER_HOST_ADDRESS(vulkan);
FN_MOON_BUFFER_DEVICE_ADDRESS(vulkan);
FN_MOON_TLAS_DEVICE_ADDRESS(vulkan);
FN_MOON_BLAS_DEVICE_ADDRESS(vulkan);

FN_MOON_DESTROY_BUFFER(vulkan);
FN_MOON_DESTROY_TEXTURE(vulkan);
FN_MOON_DESTROY_TEXTURE_VIEW(vulkan);
FN_MOON_DESTROY_SAMPLER(vulkan);
FN_MOON_DESTROY_TLAS(vulkan);
FN_MOON_DESTROY_BLAS(vulkan);

FN_MOON_TIMELINE_QUERY_POOL_ASSEMBLY(vulkan);
FN_MOON_TIMELINE_QUERY_POOL_DESTRUCTOR(vulkan);
FN_MOON_TIMELINE_SEMAPHORE_ASSEMBLY(vulkan);
FN_MOON_TIMELINE_SEMAPHORE_DESTRUCTOR(vulkan);
FN_MOON_BINARY_SEMAPHORE_ASSEMBLY(vulkan);
FN_MOON_BINARY_SEMAPHORE_DESTRUCTOR(vulkan);

FN_MOON_COMPUTE_PIPELINES_ASSEMBLY(vulkan);
FN_MOON_COMPUTE_PIPELINE_DESTRUCTOR(vulkan);
FN_MOON_WORK_GRAPH_PIPELINES_ASSEMBLY(vulkan);
FN_MOON_WORK_GRAPH_PIPELINE_DESTRUCTOR(vulkan);
FN_MOON_RASTER_PIPELINES_ASSEMBLY(vulkan);
FN_MOON_RASTER_PIPELINE_DESTRUCTOR(vulkan);
FN_MOON_RAY_TRACING_PIPELINES_ASSEMBLY(vulkan);
FN_MOON_RAY_TRACING_PIPELINE_DESTRUCTOR(vulkan);

FN_MOON_SWAPCHAIN_ASSEMBLY(vulkan);
FN_MOON_SWAPCHAIN_DESTRUCTOR(vulkan);
FN_MOON_SWAPCHAIN_WAIT_FOR_NEXT_FRAME(vulkan);
FN_MOON_SWAPCHAIN_ACQUIRE_NEXT_IMAGE(vulkan);
FN_MOON_SWAPCHAIN_CURRENT_ACQUIRE_SEMAPHORE(vulkan);
FN_MOON_SWAPCHAIN_CURRENT_PRESENT_SEMAPHORE(vulkan);
FN_MOON_SWAPCHAIN_CURRENT_CPU_TIMELINE_VALUE(vulkan);
FN_MOON_SWAPCHAIN_CURRENT_TIMELINE_PAIR(vulkan);
FN_MOON_SWAPCHAIN_GPU_TIMELINE_SEMAPHORE(vulkan);
FN_MOON_SWAPCHAIN_SET_PRESENT_MODE(vulkan);
FN_MOON_SWAPCHAIN_RESIZE(vulkan);

FN_MOON_COMMAND_RECORDER_ASSEMBLY(vulkan);
FN_MOON_COMMAND_RECORDER_DESTRUCTOR(vulkan);
FN_MOON_STAGED_COMMAND_LIST_ASSEMBLY(vulkan);
FN_MOON_STAGED_COMMAND_LIST_DESTRUCTOR(vulkan);

FN_MOON_CMD_COPY_BUFFER(vulkan);
FN_MOON_CMD_COPY_BUFFER_TO_TEXTURE(vulkan);
FN_MOON_CMD_COPY_TEXTURE_TO_BUFFER(vulkan);
FN_MOON_CMD_COPY_TEXTURE(vulkan);
FN_MOON_CMD_BLIT_TEXTURE(vulkan);
FN_MOON_CMD_RESOLVE_TEXTURE(vulkan);
FN_MOON_CMD_CLEAR_BUFFER(vulkan);
FN_MOON_CMD_CLEAR_TEXTURE(vulkan);
FN_MOON_CMD_DESTROY_BUFFER_DEFERRED(vulkan);
FN_MOON_CMD_DESTROY_TEXTURE_DEFERRED(vulkan);
FN_MOON_CMD_DESTROY_TEXTURE_VIEW_DEFERRED(vulkan);
FN_MOON_CMD_DESTROY_SAMPLER_DEFERRED(vulkan);
FN_MOON_CMD_BUILD_ACCELERATION_STRUCTURES(vulkan);
FN_MOON_CMD_ROOT_CONSTANTS(vulkan);
FN_MOON_CMD_SET_COMPUTE_PIPELINE(vulkan);
FN_MOON_CMD_SET_RASTER_PIPELINE(vulkan);
FN_MOON_CMD_SET_RAY_TRACING_PIPELINE(vulkan);
FN_MOON_CMD_SET_WORK_GRAPH_PIPELINE(vulkan);
FN_MOON_CMD_SET_VIEWPORT(vulkan);
FN_MOON_CMD_SET_SCISSOR(vulkan);
FN_MOON_CMD_SET_RASTERIZATION_SAMPLES(vulkan);
FN_MOON_CMD_SET_DEPTH_BIAS(vulkan);
FN_MOON_CMD_SET_INDEX_BUFFER(vulkan);
FN_MOON_CMD_BARRIERS_AND_TRANSITIONS(vulkan);
FN_MOON_CMD_BEGIN_RENDERPASS(vulkan);
FN_MOON_CMD_END_RENDERPASS(vulkan);
FN_MOON_CMD_WRITE_TIMESTAMPS(vulkan);
FN_MOON_CMD_RESOLVE_TIMESTAMPS(vulkan);
FN_MOON_CMD_BEGIN_LABEL(vulkan);
FN_MOON_CMD_END_LABEL(vulkan);
FN_MOON_CMD_DISPATCH(vulkan);
FN_MOON_CMD_DISPATCH_INDIRECT(vulkan);
FN_MOON_CMD_DISPATCH_GRAPH(vulkan);
FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT(vulkan);
FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT_COUNT(vulkan);
FN_MOON_CMD_DRAW(vulkan);
FN_MOON_CMD_DRAW_INDEXED(vulkan);
FN_MOON_CMD_DRAW_INDIRECT(vulkan);
FN_MOON_CMD_DRAW_INDIRECT_COUNT(vulkan);
FN_MOON_CMD_DRAW_MESH_TASKS(vulkan);
FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT(vulkan);
FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT(vulkan);
FN_MOON_CMD_TRACE_RAYS(vulkan);
FN_MOON_CMD_TRACE_RAYS_INDIRECT(vulkan);

#ifndef VK_NO_PROTOTYPES
    #define VK_NO_PROTOTYPES
#endif
#ifndef VK_ENABLE_BETA_EXTENSIONS
    #define VK_ENABLE_BETA_EXTENSIONS
#endif

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    /* to avoid including windows.h */
    #ifndef UNICODE
        #define UNICODE 1
    #endif
    #ifndef _UNICODE
        #define _UNICODE 1
    #endif
    #ifndef VC_EXTRALEAN
        #define VC_EXTRALEAN
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOGDI
        #define NOGDI
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    #include <vulkan/vk_platform.h>
    #include <vulkan/vulkan_core.h>

    typedef unsigned long                DWORD;
    typedef const wchar_t               *LPCWSTR;
    typedef void                        *HANDLE;
    typedef struct HINSTANCE__          *HINSTANCE;
    typedef struct HWND__               *HWND;
    typedef struct HMONITOR__           *HMONITOR;
    typedef struct _SECURITY_ATTRIBUTES _SECURITY_ATTRIBUTES;
    
    #include <vulkan/vulkan_win32.h>
#else
    #include <vulkan/vulkan.h>
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    #include <vulkan/vulkan_beta.h>
#endif

#include <vk_video/vulkan_video_codec_av1std_decode.h>
#include <vk_video/vulkan_video_codec_av1std_encode.h>
#include <vk_video/vulkan_video_codec_h264std_decode.h>
#include <vk_video/vulkan_video_codec_h264std_encode.h>
#include <vk_mem_alloc.h>

struct queue_family {
    s16 queue_count;
    s16 vk_index;
};

struct queue_impl {
    moon_queue_type     queue_type;
    s16                 queue_idx;
    s32                 vk_queue_family_idx;
    VkQueue             vk_queue;
    VkSemaphore         gpu_local_timeline;
    atomic_u64          latest_pending_submit_timeline_value;
};

/** Information about capabilities of accelerated video coding. */
struct physical_device_video_capability {
    VkVideoProfileInfoKHR                                           profile;
    VkVideoDecodeCapabilitiesKHR                                    decode_capabilities;
    VkVideoEncodeCapabilitiesKHR                                    encode_capabilities;
    VkVideoCapabilitiesKHR                                          capabilities;
};

/** Information about the physical device AV1 video codec. */
struct physical_device_video_av1 {
    VkVideoDecodeAV1ProfileInfoKHR                                  decode_profile;
    VkVideoDecodeAV1CapabilitiesKHR                                 decode_capabilities;
    VkVideoEncodeAV1ProfileInfoKHR                                  encode_profile;
    VkVideoEncodeAV1CapabilitiesKHR                                 encode_capabilities;
    struct physical_device_video_capability                         video_capability;
};

/** Information about the physical device H.264 video codec. */
struct physical_device_video_h264 {
    VkVideoDecodeH264ProfileInfoKHR                                 decode_profile;
    VkVideoDecodeH264CapabilitiesKHR                                decode_capabilities;
    VkVideoEncodeH264ProfileInfoKHR                                 encode_profile;
    VkVideoEncodeH264CapabilitiesKHR                                encode_capabilities;
    struct physical_device_video_capability                         video_capability;
};

/** Information about hardware properties of the physical device. */
struct physical_device_properties {
    VkPhysicalDeviceShaderEnqueuePropertiesAMDX                     shader_enqueue;
    VkPhysicalDeviceAccelerationStructurePropertiesKHR              acceleration_structure;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR                 ray_tracing_pipeline;
    VkPhysicalDeviceRayTracingInvocationReorderPropertiesNV         ray_tracing_invocation_reorder;
    VkPhysicalDeviceFragmentShadingRatePropertiesKHR                fragment_shading_rate;
    VkPhysicalDeviceMeshShaderPropertiesEXT                         mesh_shader;
    VkPhysicalDeviceRobustness2PropertiesEXT                        robustness2;
    VkPhysicalDeviceDescriptorIndexingProperties                    descriptor_indexing;
    VkPhysicalDeviceSubgroupSizeControlProperties                   subgroup_size_control;
    VkPhysicalDeviceTimelineSemaphoreProperties                     timeline_semaphore;
    VkPhysicalDeviceMaintenance6Properties                          maintenance6;
    VkPhysicalDeviceMaintenance5Properties                          maintenance5;
    VkPhysicalDeviceMaintenance4Properties                          maintenance4;
    VkPhysicalDeviceProperties2                                     properties2;
    /* memory properties */
    VkPhysicalDeviceMemoryBudgetPropertiesEXT                       memory_budget;
    VkPhysicalDeviceMemoryProperties2                               memory_properties2;
};

/** Information about features supported by the physical device. */
struct physical_device_features {
    VkPhysicalDeviceShaderEnqueueFeaturesAMDX                       shader_enqueue;
    VkPhysicalDeviceAccelerationStructureFeaturesKHR                acceleration_structure;
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR                   ray_tracing_pipeline;
    VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR              ray_tracing_position_fetch;
    VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV           ray_tracing_invocation_reorder;
    VkPhysicalDeviceRayQueryFeaturesKHR                             ray_query;
    VkPhysicalDeviceFragmentShadingRateFeaturesKHR                  fragment_shading_rate;
    VkPhysicalDeviceMeshShaderFeaturesEXT                           mesh_shader;
    VkPhysicalDeviceRobustness2FeaturesEXT                          robustness2;
    VkPhysicalDeviceDescriptorIndexingFeatures                      descriptor_indexing;
    VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT               shader_image_atomic_int64;
    VkPhysicalDeviceShaderAtomicFloatFeaturesEXT                    shader_atomic_float;
    VkPhysicalDeviceShaderAtomicInt64Features                       shader_atomic_int64;
    VkPhysicalDeviceShaderFloat16Int8Features                       shader_float16_int8;
    VkPhysicalDeviceVulkanMemoryModelFeatures                       vulkan_memory_model;
    VkPhysicalDevice16BitStorageFeatures                            bit16_storage;
    VkPhysicalDevice8BitStorageFeatures                             bit8_storage;
    VkPhysicalDeviceExtendedDynamicState3FeaturesEXT                extended_dynamic_state3;
    VkPhysicalDeviceVariablePointerFeatures                         variable_pointer;
    VkPhysicalDeviceHostQueryResetFeatures                          host_query_reset;
    VkPhysicalDeviceBufferDeviceAddressFeatures                     buffer_device_address;
    VkPhysicalDeviceScalarBlockLayoutFeatures                       scalar_block_layout;
    VkPhysicalDeviceSubgroupSizeControlFeatures                     subgroup_size_control;
    VkPhysicalDeviceMultiviewFeatures                               multiview;
    VkPhysicalDeviceTimelineSemaphoreFeatures                       timeline_semaphore;
    VkPhysicalDeviceSynchronization2Features                        synchronization2;
    VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT    dynamic_rendering_unused_attachments;
    VkPhysicalDeviceDynamicRenderingLocalReadFeatures               dynamic_rendering_local_read;
    VkPhysicalDeviceDynamicRenderingFeatures                        dynamic_rendering;
    VkPhysicalDeviceMaintenance6Features                            maintenance6;
    VkPhysicalDeviceMaintenance5Features                            maintenance5;
    VkPhysicalDeviceMaintenance4Features                            maintenance4;
    VkPhysicalDeviceFeatures2                                       features2;
    bool has_swapchain, has_conservative_rasterization; 
    bool has_video_decode_queue, has_video_encode_queue;
};

struct physical_device {
    moon_device_details                     details;
    VkPhysicalDevice                        vk_physical_device;
    u64                                     extension_bits;

    struct queue_family                     queue_families[moon_queue_type_count];
    s32                                     valid_queue_family_indices[moon_queue_type_count];
    VkQueueFlags                            main_queue_command_support;
    u32                                     found_queue_families;

    struct physical_device_properties       vk_properties;
    struct physical_device_features         vk_features;
    struct physical_device_video_av1        vk_video_av1;
    struct physical_device_video_h264       vk_video_h264;
};

struct device_heap_zombie { 
    VmaAllocation                           vma_allocation;
};

struct timeline_query_pool_zombie { 
    VkQueryPool                             vk_query_pool;
};

struct semaphore_zombie { 
    VkSemaphore                             vk_semaphore;
};

struct pipeline_zombie { 
    VkPipeline                              vk_pipeline;
};

struct command_recorder_zombie { 
    moon_queue_type                         queue_family;
    VkCommandPool                           vk_cmd_pool;
    lake_darray_t(VkCommandBuffer)          allocated_commnd_buffers;
};

struct submit_zombie {
    lake_darray_t(moon_binary_semaphore)    binary_semaphores;
    lake_darray_t(moon_timeline_semaphore)  timeline_semaphores;
};

struct buffer_impl_slot {
    moon_buffer_assembly                    assembly;
    VkBuffer                                vk_buffer;
    VmaAllocation                           vma_allocation;
    moon_device_heap                        optional_heap;
    VkDeviceAddress                         device_address;
    void                                   *host_address;
};

struct texture_view_impl_slot {
    moon_texture_view_assembly              assembly;
    VkImageView                             vk_image_view;
};

struct texture_impl_slot {
    struct texture_view_impl_slot           view_slot;
    moon_texture_assembly                   assembly;
    VkImage                                 vk_image;
    VmaAllocation                           vma_allocation;
    moon_device_heap                        optional_heap;
    s32                                     swapchain_image_idx;
    VkImageAspectFlags                      aspect_flags;
};
static constexpr s32 NOT_OWNED_BY_SWAPCHAIN = -1;

struct sampler_impl_slot {
    moon_sampler_assembly                   assembly;
    VkSampler                               vk_sampler;
};

struct tlas_impl_slot {
    moon_tlas_assembly                      assembly;
    VkAccelerationStructureKHR              vk_acceleration_structure;
    VkBuffer                                vk_buffer;
    moon_buffer_id                          buffer_id;
    u64                                     offset;
    VkDeviceAddress                         device_address;
    bool                                    owns_buffer;
};

struct blas_impl_slot {
    moon_blas_assembly                      assembly;
    VkAccelerationStructureKHR              vk_acceleration_structure;
    VkBuffer                                vk_buffer;
    moon_buffer_id                          buffer_id;
    u64                                     offset;
    VkDeviceAddress                         device_address;
    bool                                    owns_buffer;
};

struct gpu_resource_table {
    lake_spinlock                           lifetime_lock;
    /* TODO replace darrays with a specialized allocator */
    lake_darray_t(struct buffer_impl_slot)  buffer_slots;
    lake_darray_t(struct texture_impl_slot) texture_slots;
    lake_darray_t(struct sampler_impl_slot) sampler_slots;
    lake_darray_t(struct tlas_impl_slot)    tlas_slots;
    lake_darray_t(struct blas_impl_slot)    blas_slots;

    VkDescriptorSetLayout                   vk_descriptor_set_layout;
    VkDescriptorSet                         vk_descriptor_set;
    VkDescriptorPool                        vk_descriptor_pool;

    /** Contains pipeline layouts with varying push constant range size.
     *  The first size is 0 word, second is 1 word, all others are a power of 2. */
    VkPipelineLayout                        pipeline_layouts[MOON_PIPELINE_LAYOUT_COUNT];
};

struct moon_device_heap_impl {
    moon_device_heap_header                 header;
    VmaAllocationInfo                       vma_allocation_info;
    VmaAllocation                           vma_allocation;
};

struct moon_timeline_query_pool_impl {
    moon_timeline_query_pool_header         header;
    VkQueryPool                             vk_query_pool;
};

struct moon_timeline_semaphore_impl {
    moon_timeline_semaphore_header          header;
    VkSemaphore                             vk_semaphore;
};

struct moon_binary_semaphore_impl {
    moon_binary_semaphore_header            header;
    VkSemaphore                             vk_semaphore;
};

struct moon_compute_pipeline_impl {
    moon_compute_pipeline_header            header;
    VkPipeline                              vk_pipeline;
    VkPipelineLayout                        vk_pipeline_layout;
};

struct moon_work_graph_pipeline_impl {
    moon_work_graph_pipeline_header         header;
    VkPipeline                              vk_pipeline;
    VkPipelineLayout                        vk_pipeline_layout;
    usize                                   node_count;
    moon_work_graph_node const             *nodes;
};

struct moon_raster_pipeline_impl {
    moon_raster_pipeline_header             header;
    VkPipeline                              vk_pipeline;
    VkPipelineLayout                        vk_pipeline_layout;
};

struct moon_ray_tracing_pipeline_impl {
    moon_ray_tracing_pipeline_header        header;
    VkPipeline                              vk_pipeline;
    VkPipelineLayout                        vk_pipeline_layout;
    usize                                   shader_group_count;
    moon_ray_tracing_shader_group const    *shader_groups;
};

/** Every frame we get a swapchain image index. This index can be non sequential in the case 
 *  of mail box presentation and other modes. This means we need to acquire a new index every 
 *  frame to know what swapchain image to use. Semaphores in acquire MUST be un-signaled when 
 *  recording actions ON THE CPU. Because of this, we need frames_in_flight+1 semaphores for 
 *  the acquire. And so, we use two types of semaphores here: acquire and present. 
 *
 *  The present semaphore is signaled in the last submission that uses the swapchain image 
 *  and waited on in the present. The acquire semaphore is signaled when the swapchain 
 *  image is ready to be used. This also means that the previous presentation of the image 
 *  is finished and the semaphore used in the present is un-signaled.
 *
 *  I don't know of any other way to know when a present finishes (or the corresponding 
 *  semaphore is un-signaled). In order to be able to reuse semaphores used in presentation,
 *  one MUST pair them with the image they are used to present.
 *
 *  One can then rely on the acquire semaphore of the image being signaled to indicate that 
 *  the present semaphore is able to be reused, as a swapchain images acquire semaphore is 
 *  signaled when it is available and its previous present has completed.
 *
 *  In order to reuse the acquire semaphore, we must set a limit in frames in flight and 
 *  wait on the cpu to limit the frames in flight. When we have this wait in place, we can 
 *  safely reuse the acquire semaphores with a linearly increasing index corresponding to 
 *  the frame. This means the acquire semaphores are not tied to the number of swapchain 
 *  images like present semaphores, but to the number of frames in flight.
 *
 *  To limit the frames in flight we may employ a timeline semaphore that must be signaled 
 *  in a submission that uses or after one that uses the swapchain image. */
struct moon_swapchain_impl {
    moon_swapchain_header                   header;
    /** The swapchain handle. */
    VkSwapchainKHR                          vk_swapchain;
    /** A Vulkan surface created from a native window. */
    VkSurfaceKHR                            vk_surface;
    /** The selected surface format. */
    VkSurfaceFormatKHR                      vk_surface_format;
    /** The window framebuffer resolution. */
    VkExtent2D                              vk_surface_extent;
    /** Abstracted presentation modes, acquired from the Vulkan surface. */
    lake_darray_t(moon_present_mode)        supported_present_modes;
    /** Swapchain holds strong references to these images, as it owns them. */
    lake_darray_t(moon_texture_id)          images;
    /** Signaled in the last submission that uses the swapchain image. */
    lake_darray_t(moon_binary_semaphore)    present_semaphores;
    /** Signaled when the swapchain image is ready to be used. */
    lake_darray_t(moon_binary_semaphore)    acquire_semaphores;
    /** cpu_timeline % frames_in_flight, used to access the acquire semaphore. */
    usize                                   acquire_semaphore_idx;
    /** Monotonically increasing timeline value. */
    u64                                     cpu_timeline;
    /** Timeline semaphore used to track how far behind the GPU is. */
    moon_timeline_semaphore                 gpu_timeline;
    /** This presentation mode will be used if V-SYNC is disabled. */
    VkPresentModeKHR                        no_vsync_present_mode;
    /** This is the swapchain image index that acquire returns. This is not necessarily linear.
     *  This index must be used for present semaphores, as they are paired to the images. */
    u32                                     current_image_idx;
};

static constexpr s8 DEFERRED_DESTRUCTION_BUFFER_IDX = 0;
static constexpr s8 DEFERRED_DESTRUCTION_TEXTURE_IDX = 1;
static constexpr s8 DEFERRED_DESTRUCTION_TEXTURE_VIEW_IDX = 2;
static constexpr s8 DEFERRED_DESTRUCTION_SAMPLER_IDX = 3;
static constexpr s8 DEFERRED_DESTRUCTION_TIMELINE_QUERY_POOL_IDX = 4;
static constexpr usize COMMAND_LIST_BARRIER_MAX_BATCH_SIZE = 16;
static constexpr usize COMMAND_LIST_COLOR_ATTACHMENT_MAX = 16;

struct command_pool_arena {
    lake_darray_t(VkCommandPool)        pools_and_buffers;
    s32                                 queue_family_idx;
    lake_spinlock                       spinlock;
};

struct staged_command_list_data {
    VkCommandBuffer                     vk_cmd_buffer;
    lake_darray_t(lake_pair(u64, u8))   deferred_destructions;
    lake_darray_t(moon_buffer_id)       used_buffers;
    lake_darray_t(moon_texture_id)      used_textures;
    lake_darray_t(moon_texture_view_id) used_texture_views;
    lake_darray_t(moon_sampler_id)      used_samplers;
    lake_darray_t(moon_tlas_id)         used_tlass;
    lake_darray_t(moon_blas_id)         used_blass;
};

struct moon_command_recorder_impl {
    moon_command_recorder_header        header;
    VkCommandPool                       vk_cmd_pool;
    lake_darray_t(VkCommandBuffer)      allocated_command_buffers; 
    VkMemoryBarrier2                    memory_barrier_batch[COMMAND_LIST_BARRIER_MAX_BATCH_SIZE];
    VkImageMemoryBarrier2               image_barrier_batch[COMMAND_LIST_BARRIER_MAX_BATCH_SIZE];
    u32                                 memory_barrier_batch_count;
    u32                                 image_barrier_batch_count;
    u32                                 split_barrier_batch_count;
    bool                                in_renderpass;
    enum : s8 {
        COMMAND_RECORDER_NO_PIPELINE = 0,
        COMMAND_RECORDER_COMPUTE_PIPELINE,
        COMMAND_RECORDER_WORK_GRAPH_PIPELINE,
        COMMAND_RECORDER_RASTER_PIPELINE,
        COMMAND_RECORDER_RAY_TRACING_PIPELINE,
    } current_pipeline_variant;
    union {
        struct {}                      *no_pipeline;
        moon_compute_pipeline           compute;
        moon_work_graph_pipeline        work_graph;
        moon_raster_pipeline            raster;
        moon_ray_tracing_pipeline       ray_tracing;
    } current_pipeline;
    struct staged_command_list_data     current_command_data;
};

struct moon_staged_command_list_impl {
    moon_staged_command_list_header     header;
    moon_command_recorder               cmd_recorder;
    struct staged_command_list_data     data;
};

struct moon_adapter_impl {
    moon_interface_impl                     interface;
    /** An array of physical device details, queried once at initialization. */
    lake_darray_t(struct physical_device)   physical_devices;
    /** An instance makes Vulkan functions available to us. It is used for calls to the driver,
     *  and holds information about the application. Afterwards it is passed to the logical device. */
    VkInstance                              vk_instance;
    /** Used at debug builds with validation layers enabled. */
    VkDebugUtilsMessengerEXT                vk_debug_messenger;
    /** Allocation callbacks passes into Vulkan API calls. */
    VkAllocationCallbacks                   vk_allocator;
    /** The loaded driver library. */
    void                                   *vulkan_library;
    /** API version of the Vulkan instance. */
    u32                                     api_version;

    /* access points */
    PFN_vkGetInstanceProcAddr                                   vkGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr                                     vkGetDeviceProcAddr;

    /* global procedures */
    PFN_vkCreateInstance                                        vkCreateInstance;
    PFN_vkEnumerateInstanceExtensionProperties                  vkEnumerateInstanceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties                      vkEnumerateInstanceLayerProperties;
    PFN_vkEnumerateInstanceVersion                              vkEnumerateInstanceVersion;

    /* instance procedures */
    PFN_vkCreateDevice                                          vkCreateDevice;
    PFN_vkDestroyInstance                                       vkDestroyInstance;
    PFN_vkEnumerateDeviceExtensionProperties                    vkEnumerateDeviceExtensionProperties;
    PFN_vkEnumeratePhysicalDevices                              vkEnumeratePhysicalDevices;
    PFN_vkEnumeratePhysicalDeviceGroups                         vkEnumeratePhysicalDeviceGroups;
    PFN_vkGetPhysicalDeviceFeatures                             vkGetPhysicalDeviceFeatures;
    PFN_vkGetPhysicalDeviceFeatures2                            vkGetPhysicalDeviceFeatures2;
    PFN_vkGetPhysicalDeviceFormatProperties                     vkGetPhysicalDeviceFormatProperties;
    PFN_vkGetPhysicalDeviceFormatProperties2                    vkGetPhysicalDeviceFormatProperties2;
    PFN_vkGetPhysicalDeviceImageFormatProperties                vkGetPhysicalDeviceImageFormatProperties;
    PFN_vkGetPhysicalDeviceImageFormatProperties2               vkGetPhysicalDeviceImageFormatProperties2;
    PFN_vkGetPhysicalDeviceMemoryProperties                     vkGetPhysicalDeviceMemoryProperties;
    PFN_vkGetPhysicalDeviceMemoryProperties2                    vkGetPhysicalDeviceMemoryProperties2;
    PFN_vkGetPhysicalDeviceProperties                           vkGetPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceProperties2                          vkGetPhysicalDeviceProperties2;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties                vkGetPhysicalDeviceQueueFamilyProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2               vkGetPhysicalDeviceQueueFamilyProperties2;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties          vkGetPhysicalDeviceSparseImageFormatProperties;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties2         vkGetPhysicalDeviceSparseImageFormatProperties2;
    PFN_vkGetPhysicalDeviceToolProperties                       vkGetPhysicalDeviceToolProperties;

    /* surface */
    PFN_vkDestroySurfaceKHR                                     vkDestroySurfaceKHR;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR                    vkGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR               vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR                    vkGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR               vkGetPhysicalDeviceSurfacePresentModesKHR;

    /* debug utils */
    PFN_vkSetDebugUtilsObjectNameEXT                            vkSetDebugUtilsObjectNameEXT;
    PFN_vkSetDebugUtilsObjectTagEXT                             vkSetDebugUtilsObjectTagEXT;
    PFN_vkQueueBeginDebugUtilsLabelEXT                          vkQueueBeginDebugUtilsLabelEXT;
    PFN_vkQueueEndDebugUtilsLabelEXT                            vkQueueEndDebugUtilsLabelEXT;
    PFN_vkQueueInsertDebugUtilsLabelEXT                         vkQueueInsertDebugUtilsLabelEXT;
    PFN_vkCmdBeginDebugUtilsLabelEXT                            vkCmdBeginDebugUtilsLabelEXT;
    PFN_vkCmdEndDebugUtilsLabelEXT                              vkCmdEndDebugUtilsLabelEXT;
    PFN_vkCmdInsertDebugUtilsLabelEXT                           vkCmdInsertDebugUtilsLabelEXT;
    PFN_vkCreateDebugUtilsMessengerEXT                          vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT                         vkDestroyDebugUtilsMessengerEXT;
    PFN_vkSubmitDebugUtilsMessageEXT                            vkSubmitDebugUtilsMessageEXT;

    /* video coding */
    PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR                 vkGetPhysicalDeviceVideoCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR             vkGetPhysicalDeviceVideoFormatPropertiesKHR;
    PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR;
};

struct moon_device_impl {
    moon_device_header                      header;
    /** The Vulkan context of a rendering device, using the given physical device. */
    VkDevice                                vk_device;
    /** The details of the physical device used to construct this rendering device. */
    struct physical_device const           *physical_device;
    /** Points into the allocation callbacks in the adapter structure. */
    VkAllocationCallbacks const            *host_allocator;
    /** GPU allocator from the VulkanMemoryAllocator library. */
    VmaAllocator                            gpu_allocator;
    
    /** Device address buffer for a bindless descriptor set design. */
    VkBuffer                                device_address_buffer;
    u64                                    *device_address_buffer_host;
    VmaAllocation                           device_address_buffer_allocation;

    /** 'NULL' resources, used to fill empty slots in the resource table after a resource is destroyed.
     *  This is not necessary, as it is valid to have "garbage" in the descriptor slots given our 
     *  enabled Vulkan features. BUT, accessing garbage descriptors normally causes a device lost 
     *  immediately, making debugging much harder. So instead of leaving dead descriptors dangle, 
     *  we overwrite them with 'NULL' descriptors that just contain some debug value (e.g. pink 
     *  0xFF00FFFF). This in particular prevents device hang in the case of a use after free if 
     *  the device does not encounter a race condition on the descriptor update before. */
    VkBuffer                                null_buffer;
    VkImage                                 null_image;
    VkImageView                             null_image_view;
    VkSampler                               null_sampler;
    VmaAllocation                           null_buffer_allocation;
    VmaAllocation                           null_image_allocation;

    /** Every submit to any queue increments the global submit timeline.
     *  Each queue stores a mapping between local submit index and global submit index for each 
     *  of their in flight submits. When destroying a resource it becomes a zombie, the zombie 
     *  remembers the current global timeline value. When deferred disassembly is called, the 
     *  zombies timeline values are compared against submits running in all queues. If the 
     *  zombies global submit index is smaller than the global index of all submits currently 
     *  in flight (on all queues), we can safely clean the resource up. */
    atomic_u64                              submit_timeline;
    lake_spinlock                           zombies_locks[10];
    /** Pairs of resource handles or zombies with submit timeline values. */
    lake_deque_t(lake_pair(u64, struct command_recorder_zombie))    command_recorder_zombies;
    lake_deque_t(lake_pair(u64, moon_buffer_id))                    buffer_zombies;
    lake_deque_t(lake_pair(u64, moon_texture_id))                   texture_zombies;
    lake_deque_t(lake_pair(u64, moon_texture_view_id))              texture_view_zombies;
    lake_deque_t(lake_pair(u64, moon_sampler_id))                   sampler_zombies;
    lake_deque_t(lake_pair(u64, moon_tlas_id))                      tlas_zombies;
    lake_deque_t(lake_pair(u64, moon_blas_id))                      blas_zombies;
    lake_deque_t(lake_pair(u64, struct semaphore_zombie))           semaphore_zombies;
    lake_deque_t(lake_pair(u64, struct timeline_query_pool_zombie)) timeline_query_pool_zombies;
    lake_deque_t(lake_pair(u64, struct device_heap_zombie))         heap_zombies;

    /** Command buffer/pool recycling, accessed via `moon_queue_type`. */
    struct command_pool_arena               command_pool_arenas[moon_queue_type_count];
    /** Command queues in order: main, compute (8), transfer (2), sparse binding, video (2). */
    struct queue_impl                       queues[MOON_QUEUE_INDEX_COUNT];
    /** Table of GPU resource objects. */
    struct gpu_resource_table               gpu_resource_table;

    /* core 1.0 */
	PFN_vkAllocateCommandBuffers                                vkAllocateCommandBuffers;
	PFN_vkAllocateDescriptorSets                                vkAllocateDescriptorSets;
	PFN_vkAllocateMemory                                        vkAllocateMemory;
	PFN_vkBeginCommandBuffer                                    vkBeginCommandBuffer;
	PFN_vkBindBufferMemory                                      vkBindBufferMemory;
	PFN_vkBindImageMemory                                       vkBindImageMemory;
	PFN_vkCmdBeginQuery                                         vkCmdBeginQuery;
	PFN_vkCmdBeginRenderPass                                    vkCmdBeginRenderPass;
	PFN_vkCmdBindDescriptorSets                                 vkCmdBindDescriptorSets;
	PFN_vkCmdBindIndexBuffer                                    vkCmdBindIndexBuffer;
	PFN_vkCmdBindPipeline                                       vkCmdBindPipeline;
	PFN_vkCmdBindVertexBuffers                                  vkCmdBindVertexBuffers;
	PFN_vkCmdBlitImage                                          vkCmdBlitImage;
	PFN_vkCmdClearAttachments                                   vkCmdClearAttachments;
	PFN_vkCmdClearColorImage                                    vkCmdClearColorImage;
	PFN_vkCmdClearDepthStencilImage                             vkCmdClearDepthStencilImage;
	PFN_vkCmdCopyBuffer                                         vkCmdCopyBuffer;
	PFN_vkCmdCopyBufferToImage                                  vkCmdCopyBufferToImage;
	PFN_vkCmdCopyImage                                          vkCmdCopyImage;
	PFN_vkCmdCopyImageToBuffer                                  vkCmdCopyImageToBuffer;
	PFN_vkCmdCopyQueryPoolResults                               vkCmdCopyQueryPoolResults;
	PFN_vkCmdDispatch                                           vkCmdDispatch;
	PFN_vkCmdDispatchIndirect                                   vkCmdDispatchIndirect;
	PFN_vkCmdDraw                                               vkCmdDraw;
	PFN_vkCmdDrawIndexed                                        vkCmdDrawIndexed;
	PFN_vkCmdDrawIndexedIndirect                                vkCmdDrawIndexedIndirect;
	PFN_vkCmdDrawIndirect                                       vkCmdDrawIndirect;
	PFN_vkCmdEndQuery                                           vkCmdEndQuery;
	PFN_vkCmdEndRenderPass                                      vkCmdEndRenderPass;
	PFN_vkCmdExecuteCommands                                    vkCmdExecuteCommands;
	PFN_vkCmdFillBuffer                                         vkCmdFillBuffer;
	PFN_vkCmdNextSubpass                                        vkCmdNextSubpass;
	PFN_vkCmdPipelineBarrier                                    vkCmdPipelineBarrier;
	PFN_vkCmdPushConstants                                      vkCmdPushConstants;
	PFN_vkCmdResetEvent                                         vkCmdResetEvent;
	PFN_vkCmdResetQueryPool                                     vkCmdResetQueryPool;
	PFN_vkCmdResolveImage                                       vkCmdResolveImage;
	PFN_vkCmdSetBlendConstants                                  vkCmdSetBlendConstants;
	PFN_vkCmdSetDepthBias                                       vkCmdSetDepthBias;
	PFN_vkCmdSetDepthBounds                                     vkCmdSetDepthBounds;
	PFN_vkCmdSetEvent                                           vkCmdSetEvent;
	PFN_vkCmdSetLineWidth                                       vkCmdSetLineWidth;
	PFN_vkCmdSetScissor                                         vkCmdSetScissor;
	PFN_vkCmdSetStencilCompareMask                              vkCmdSetStencilCompareMask;
	PFN_vkCmdSetStencilReference                                vkCmdSetStencilReference;
	PFN_vkCmdSetStencilWriteMask                                vkCmdSetStencilWriteMask;
	PFN_vkCmdSetViewport                                        vkCmdSetViewport;
	PFN_vkCmdUpdateBuffer                                       vkCmdUpdateBuffer;
	PFN_vkCmdWaitEvents                                         vkCmdWaitEvents;
	PFN_vkCmdWriteTimestamp                                     vkCmdWriteTimestamp;
	PFN_vkCreateBuffer                                          vkCreateBuffer;
	PFN_vkCreateBufferView                                      vkCreateBufferView;
	PFN_vkCreateCommandPool                                     vkCreateCommandPool;
	PFN_vkCreateComputePipelines                                vkCreateComputePipelines;
	PFN_vkCreateDescriptorPool                                  vkCreateDescriptorPool;
	PFN_vkCreateDescriptorSetLayout                             vkCreateDescriptorSetLayout;
	PFN_vkCreateEvent                                           vkCreateEvent;
	PFN_vkCreateFence                                           vkCreateFence;
	PFN_vkCreateFramebuffer                                     vkCreateFramebuffer;
	PFN_vkCreateGraphicsPipelines                               vkCreateGraphicsPipelines;
	PFN_vkCreateImage                                           vkCreateImage;
	PFN_vkCreateImageView                                       vkCreateImageView;
	PFN_vkCreatePipelineCache                                   vkCreatePipelineCache;
	PFN_vkCreatePipelineLayout                                  vkCreatePipelineLayout;
	PFN_vkCreateQueryPool                                       vkCreateQueryPool;
	PFN_vkCreateRenderPass                                      vkCreateRenderPass;
	PFN_vkCreateSampler                                         vkCreateSampler;
	PFN_vkCreateSemaphore                                       vkCreateSemaphore;
	PFN_vkCreateShaderModule                                    vkCreateShaderModule;
	PFN_vkDestroyBuffer                                         vkDestroyBuffer;
	PFN_vkDestroyBufferView                                     vkDestroyBufferView;
	PFN_vkDestroyCommandPool                                    vkDestroyCommandPool;
	PFN_vkDestroyDescriptorPool                                 vkDestroyDescriptorPool;
	PFN_vkDestroyDescriptorSetLayout                            vkDestroyDescriptorSetLayout;
	PFN_vkDestroyDevice                                         vkDestroyDevice;
	PFN_vkDestroyEvent                                          vkDestroyEvent;
	PFN_vkDestroyFence                                          vkDestroyFence;
	PFN_vkDestroyFramebuffer                                    vkDestroyFramebuffer;
	PFN_vkDestroyImage                                          vkDestroyImage;
	PFN_vkDestroyImageView                                      vkDestroyImageView;
	PFN_vkDestroyPipeline                                       vkDestroyPipeline;
	PFN_vkDestroyPipelineCache                                  vkDestroyPipelineCache;
	PFN_vkDestroyPipelineLayout                                 vkDestroyPipelineLayout;
	PFN_vkDestroyQueryPool                                      vkDestroyQueryPool;
	PFN_vkDestroyRenderPass                                     vkDestroyRenderPass;
	PFN_vkDestroySampler                                        vkDestroySampler;
	PFN_vkDestroySemaphore                                      vkDestroySemaphore;
	PFN_vkDestroyShaderModule                                   vkDestroyShaderModule;
	PFN_vkDeviceWaitIdle                                        vkDeviceWaitIdle;
	PFN_vkEndCommandBuffer                                      vkEndCommandBuffer;
	PFN_vkFlushMappedMemoryRanges                               vkFlushMappedMemoryRanges;
	PFN_vkFreeCommandBuffers                                    vkFreeCommandBuffers;
	PFN_vkFreeDescriptorSets                                    vkFreeDescriptorSets;
	PFN_vkFreeMemory                                            vkFreeMemory;
	PFN_vkGetBufferMemoryRequirements                           vkGetBufferMemoryRequirements;
	PFN_vkGetDeviceMemoryCommitment                             vkGetDeviceMemoryCommitment;
	PFN_vkGetDeviceQueue                                        vkGetDeviceQueue;
	PFN_vkGetEventStatus                                        vkGetEventStatus;
	PFN_vkGetFenceStatus                                        vkGetFenceStatus;
	PFN_vkGetImageMemoryRequirements                            vkGetImageMemoryRequirements;
	PFN_vkGetImageSparseMemoryRequirements                      vkGetImageSparseMemoryRequirements;
	PFN_vkGetImageSubresourceLayout                             vkGetImageSubresourceLayout;
	PFN_vkGetPipelineCacheData                                  vkGetPipelineCacheData;
	PFN_vkGetQueryPoolResults                                   vkGetQueryPoolResults;
	PFN_vkGetRenderAreaGranularity                              vkGetRenderAreaGranularity;
	PFN_vkInvalidateMappedMemoryRanges                          vkInvalidateMappedMemoryRanges;
	PFN_vkMapMemory                                             vkMapMemory;
	PFN_vkMergePipelineCaches                                   vkMergePipelineCaches;
	PFN_vkQueueBindSparse                                       vkQueueBindSparse;
	PFN_vkQueueSubmit                                           vkQueueSubmit;
	PFN_vkQueueWaitIdle                                         vkQueueWaitIdle;
	PFN_vkResetCommandBuffer                                    vkResetCommandBuffer;
	PFN_vkResetCommandPool                                      vkResetCommandPool;
	PFN_vkResetDescriptorPool                                   vkResetDescriptorPool;
	PFN_vkResetEvent                                            vkResetEvent;
	PFN_vkResetFences                                           vkResetFences;
	PFN_vkSetEvent                                              vkSetEvent;
	PFN_vkUnmapMemory                                           vkUnmapMemory;
	PFN_vkUpdateDescriptorSets                                  vkUpdateDescriptorSets;
	PFN_vkWaitForFences                                         vkWaitForFences;

    /* core 1.1 */
    PFN_vkBindBufferMemory2                                     vkBindBufferMemory2;
    PFN_vkBindImageMemory2                                      vkBindImageMemory2;
    PFN_vkCmdDispatchBase                                       vkCmdDispatchBase;
    PFN_vkCmdSetDeviceMask                                      vkCmdSetDeviceMask;
    PFN_vkCreateDescriptorUpdateTemplate                        vkCreateDescriptorUpdateTemplate;
    PFN_vkCreateSamplerYcbcrConversion                          vkCreateSamplerYcbcrConversion;
    PFN_vkDestroyDescriptorUpdateTemplate                       vkDestroyDescriptorUpdateTemplate;
    PFN_vkDestroySamplerYcbcrConversion                         vkDestroySamplerYcbcrConversion;
    PFN_vkGetBufferMemoryRequirements2                          vkGetBufferMemoryRequirements2;
    PFN_vkGetDescriptorSetLayoutSupport                         vkGetDescriptorSetLayoutSupport;
    PFN_vkGetDeviceGroupPeerMemoryFeatures                      vkGetDeviceGroupPeerMemoryFeatures;
    PFN_vkGetDeviceQueue2                                       vkGetDeviceQueue2;
    PFN_vkGetImageMemoryRequirements2                           vkGetImageMemoryRequirements2;
    PFN_vkGetImageSparseMemoryRequirements2                     vkGetImageSparseMemoryRequirements2;
    PFN_vkTrimCommandPool                                       vkTrimCommandPool;
    PFN_vkUpdateDescriptorSetWithTemplate                       vkUpdateDescriptorSetWithTemplate;

    /* core 1.2 */
    PFN_vkCmdDrawIndexedIndirectCount                           vkCmdDrawIndexedIndirectCount;
    PFN_vkCmdDrawIndirectCount                                  vkCmdDrawIndirectCount;
    PFN_vkGetBufferDeviceAddress                                vkGetBufferDeviceAddress;
    PFN_vkGetBufferOpaqueCaptureAddress                         vkGetBufferOpaqueCaptureAddress;
    PFN_vkGetDeviceMemoryOpaqueCaptureAddress                   vkGetDeviceMemoryOpaqueCaptureAddress;
    PFN_vkGetSemaphoreCounterValue                              vkGetSemaphoreCounterValue;
    PFN_vkResetQueryPool                                        vkResetQueryPool;
    PFN_vkSignalSemaphore                                       vkSignalSemaphore;
    PFN_vkWaitSemaphores                                        vkWaitSemaphores;

    /* core 1.3 */
    PFN_vkCmdBeginRendering                                     vkCmdBeginRendering;
    PFN_vkCmdEndRendering                                       vkCmdEndRendering;
    PFN_vkCmdPipelineBarrier2                                   vkCmdPipelineBarrier2;
    PFN_vkCmdResetEvent2                                        vkCmdResetEvent2;
    PFN_vkCmdSetEvent2                                          vkCmdSetEvent2;
    PFN_vkCmdWaitEvents2                                        vkCmdWaitEvents2;
    PFN_vkCmdWriteTimestamp2                                    vkCmdWriteTimestamp2;
    PFN_vkQueueSubmit2                                          vkQueueSubmit2;
    PFN_vkGetDeviceBufferMemoryRequirements                     vkGetDeviceBufferMemoryRequirements;
    PFN_vkGetDeviceImageMemoryRequirements                      vkGetDeviceImageMemoryRequirements;
    PFN_vkGetDeviceImageSparseMemoryRequirements                vkGetDeviceImageSparseMemoryRequirements;

    /* core 1.4 */
    PFN_vkCmdBindDescriptorSets2                                vkCmdBindDescriptorSets2;
    PFN_vkCmdBindIndexBuffer2                                   vkCmdBindIndexBuffer2;
    PFN_vkCmdPushConstants2                                     vkCmdPushConstants2;
    PFN_vkCmdSetRenderingAttachmentLocations                    vkCmdSetRenderingAttachmentLocations;
    PFN_vkCmdSetRenderingInputAttachmentIndices                 vkCmdSetRenderingInputAttachmentIndices;
    PFN_vkGetDeviceImageSubresourceLayout                       vkGetDeviceImageSubresourceLayout;
    PFN_vkGetImageSubresourceLayout2                            vkGetImageSubresourceLayout2;
    PFN_vkGetRenderingAreaGranularity                           vkGetRenderingAreaGranularity;

    /* swapchain */
    PFN_vkAcquireNextImageKHR                                   vkAcquireNextImageKHR;
    PFN_vkAcquireNextImage2KHR                                  vkAcquireNextImage2KHR;
    PFN_vkCreateSwapchainKHR                                    vkCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR                                   vkDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR                                 vkGetSwapchainImagesKHR;
    PFN_vkQueuePresentKHR                                       vkQueuePresentKHR;

    /* device fault */
    PFN_vkGetDeviceFaultInfoEXT                                 vkGetDeviceFaultInfoEXT;

    /* dynamic state 3 */
    PFN_vkCmdSetRasterizationSamplesEXT                         vkCmdSetRasterizationSamplesEXT;

    /* mesh shader */
    PFN_vkCmdDrawMeshTasksEXT                                   vkCmdDrawMeshTasksEXT;
    PFN_vkCmdDrawMeshTasksIndirectEXT                           vkCmdDrawMeshTasksIndirectEXT;
    PFN_vkCmdDrawMeshTasksIndirectCountEXT                      vkCmdDrawMeshTasksIndirectCountEXT;

    /* deferred host operations */
    PFN_vkCreateDeferredOperationKHR                            vkCreateDeferredOperationKHR;
    PFN_vkDeferredOperationJoinKHR                              vkDeferredOperationJoinKHR;
    PFN_vkDestroyDeferredOperationKHR                           vkDestroyDeferredOperationKHR;
    PFN_vkGetDeferredOperationMaxConcurrencyKHR                 vkGetDeferredOperationMaxConcurrencyKHR;
    PFN_vkGetDeferredOperationResultKHR                         vkGetDeferredOperationResultKHR;

    /* acceleration structure */
    PFN_vkBuildAccelerationStructuresKHR                        vkBuildAccelerationStructuresKHR;
    PFN_vkCmdBuildAccelerationStructuresIndirectKHR             vkCmdBuildAccelerationStructuresIndirectKHR;
    PFN_vkCmdBuildAccelerationStructuresKHR                     vkCmdBuildAccelerationStructuresKHR;
    PFN_vkCmdCopyAccelerationStructureKHR                       vkCmdCopyAccelerationStructureKHR;
    PFN_vkCmdCopyAccelerationStructureToMemoryKHR               vkCmdCopyAccelerationStructureToMemoryKHR;
    PFN_vkCmdCopyMemoryToAccelerationStructureKHR               vkCmdCopyMemoryToAccelerationStructureKHR;
    PFN_vkCmdWriteAccelerationStructuresPropertiesKHR           vkCmdWriteAccelerationStructuresPropertiesKHR;
    PFN_vkCopyAccelerationStructureKHR                          vkCopyAccelerationStructureKHR;
    PFN_vkCopyAccelerationStructureToMemoryKHR                  vkCopyAccelerationStructureToMemoryKHR;
    PFN_vkCopyMemoryToAccelerationStructureKHR                  vkCopyMemoryToAccelerationStructureKHR;
    PFN_vkCreateAccelerationStructureKHR                        vkCreateAccelerationStructureKHR;
    PFN_vkDestroyAccelerationStructureKHR                       vkDestroyAccelerationStructureKHR;
    PFN_vkGetAccelerationStructureBuildSizesKHR                 vkGetAccelerationStructureBuildSizesKHR;
    PFN_vkGetAccelerationStructureDeviceAddressKHR              vkGetAccelerationStructureDeviceAddressKHR;
    PFN_vkGetDeviceAccelerationStructureCompatibilityKHR        vkGetDeviceAccelerationStructureCompatibilityKHR;
    PFN_vkWriteAccelerationStructuresPropertiesKHR              vkWriteAccelerationStructuresPropertiesKHR;

    /* ray tracing pipeline */
    PFN_vkCmdSetRayTracingPipelineStackSizeKHR                  vkCmdSetRayTracingPipelineStackSizeKHR;
    PFN_vkCmdTraceRaysIndirectKHR                               vkCmdTraceRaysIndirectKHR;
    PFN_vkCmdTraceRaysIndirect2KHR                              vkCmdTraceRaysIndirect2KHR; /* ray_tracing_maintenance1 */
    PFN_vkCmdTraceRaysKHR                                       vkCmdTraceRaysKHR;
    PFN_vkCreateRayTracingPipelinesKHR                          vkCreateRayTracingPipelinesKHR;
    PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR       vkGetRayTracingCaptureReplayShaderGroupHandlesKHR;
    PFN_vkGetRayTracingShaderGroupHandlesKHR                    vkGetRayTracingShaderGroupHandlesKHR;
    PFN_vkGetRayTracingShaderGroupStackSizeKHR                  vkGetRayTracingShaderGroupStackSizeKHR;

    /* video coding */
    PFN_vkBindVideoSessionMemoryKHR                             vkBindVideoSessionMemoryKHR;
    PFN_vkCmdBeginVideoCodingKHR                                vkCmdBeginVideoCodingKHR;
    PFN_vkCmdControlVideoCodingKHR                              vkCmdControlVideoCodingKHR;
    PFN_vkCmdDecodeVideoKHR                                     vkCmdDecodeVideoKHR;
    PFN_vkCmdEncodeVideoKHR                                     vkCmdEncodeVideoKHR;
    PFN_vkCmdEndVideoCodingKHR                                  vkCmdEndVideoCodingKHR;
    PFN_vkCreateVideoSessionKHR                                 vkCreateVideoSessionKHR;
    PFN_vkCreateVideoSessionParametersKHR                       vkCreateVideoSessionParametersKHR;
    PFN_vkDestroyVideoSessionKHR                                vkDestroyVideoSessionKHR;
    PFN_vkDestroyVideoSessionParametersKHR                      vkDestroyVideoSessionParametersKHR;
    PFN_vkGetEncodedVideoSessionParametersKHR                   vkGetEncodedVideoSessionParametersKHR;
    PFN_vkGetVideoSessionMemoryRequirementsKHR                  vkGetVideoSessionMemoryRequirementsKHR;
    PFN_vkUpdateVideoSessionParametersKHR                       vkUpdateVideoSessionParametersKHR;

    /* work graph */
    PFN_vkCmdDispatchGraphAMDX                                  vkCmdDispatchGraphAMDX;
    PFN_vkCmdDispatchGraphIndirectAMDX                          vkCmdDispatchGraphIndirectAMDX;
    PFN_vkCmdDispatchGraphIndirectCountAMDX                     vkCmdDispatchGraphIndirectCountAMDX;
    PFN_vkCmdInitializeGraphScratchMemoryAMDX                   vkCmdInitializeGraphScratchMemoryAMDX;
    PFN_vkCreateExecutionGraphPipelinesAMDX                     vkCreateExecutionGraphPipelinesAMDX;
    PFN_vkGetExecutionGraphPipelineNodeIndexAMDX                vkGetExecutionGraphPipelineNodeIndexAMDX;
    PFN_vkGetExecutionGraphPipelineScratchSizeAMDX              vkGetExecutionGraphPipelineScratchSizeAMDX;
};

extern LAKE_CONST_FN char const *LAKECALL vk_result_string(VkResult result);

#ifndef LAKE_NDEBUG
    #define VERIFY_VK_ERROR(x) do { \
        VkResult res__ = (x); \
        lake_dbg_assert(res__ == VK_SUCCESS, LAKE_VALIDATION_FAILED, "Vulkan API call assertion for `%s` failed, with status: %s.", #x, vk_result_string(res__)); \
    } while(0)
#else
    #define VERIFY_VK_ERROR(x) (void)(x)
#endif

#endif /* MOON_VULKAN */
