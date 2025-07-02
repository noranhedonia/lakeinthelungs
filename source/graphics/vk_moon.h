#pragma once

#include <lake/modules/moon.h>
#include <lake/data_structures/darray.h>
#include <lake/data_structures/deque.h>
#include <lake/data_structures/mpmc.h>
#ifdef MOON_VULKAN

FN_MOON_CONNECT_TO_HADAL(vulkan);
FN_MOON_LIST_DEVICE_DETAILS(vulkan);

FN_MOON_DEVICE_ASSEMBLY(vulkan);
FN_MOON_DEVICE_ZERO_REFCNT(vulkan);
FN_MOON_DEVICE_QUEUE_COUNT(vulkan);
FN_MOON_DEVICE_QUEUE_WAIT_IDLE(vulkan);
FN_MOON_DEVICE_WAIT_IDLE(vulkan);
FN_MOON_DEVICE_SUBMIT_COMMANDS(vulkan);
FN_MOON_DEVICE_PRESENT_FRAMES(vulkan);
FN_MOON_DEVICE_COMMIT_DEFERRED_DESTRUCTORS(vulkan);
FN_MOON_DEVICE_BUFFER_MEMORY_REQUIREMENTS(vulkan);
FN_MOON_DEVICE_TEXTURE_MEMORY_REQUIREMENTS(vulkan);
FN_MOON_DEVICE_TLAS_BUILD_SIZES(vulkan);
FN_MOON_DEVICE_BLAS_BUILD_SIZES(vulkan);
FN_MOON_DEVICE_MEMORY_REPORT(vulkan);

FN_MOON_MEMORY_HEAP_ASSEMBLY(vulkan);
FN_MOON_MEMORY_HEAP_ZERO_REFCNT(vulkan);

FN_MOON_CREATE_BUFFER(vulkan);
FN_MOON_CREATE_BUFFER_FROM_MEMORY_HEAP(vulkan);
FN_MOON_CREATE_TEXTURE(vulkan);
FN_MOON_CREATE_TEXTURE_FROM_MEMORY_HEAP(vulkan);
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

FN_MOON_READ_BUFFER_ASSEMBLY(vulkan);
FN_MOON_READ_TEXTURE_ASSEMBLY(vulkan);
FN_MOON_READ_TEXTURE_VIEW_ASSEMBLY(vulkan);
FN_MOON_READ_SAMPLER_ASSEMBLY(vulkan);
FN_MOON_READ_TLAS_ASSEMBLY(vulkan);
FN_MOON_READ_BLAS_ASSEMBLY(vulkan);

FN_MOON_DESTROY_BUFFER(vulkan);
FN_MOON_DESTROY_TEXTURE(vulkan);
FN_MOON_DESTROY_TEXTURE_VIEW(vulkan);
FN_MOON_DESTROY_SAMPLER(vulkan);
FN_MOON_DESTROY_TLAS(vulkan);
FN_MOON_DESTROY_BLAS(vulkan);

FN_MOON_TIMELINE_QUERY_POOL_ASSEMBLY(vulkan);
FN_MOON_TIMELINE_QUERY_POOL_ZERO_REFCNT(vulkan);
FN_MOON_TIMELINE_QUERY_POOL_QUERY_RESULTS(vulkan);

FN_MOON_TIMELINE_SEMAPHORE_ASSEMBLY(vulkan);
FN_MOON_TIMELINE_SEMAPHORE_ZERO_REFCNT(vulkan);
FN_MOON_TIMELINE_SEMAPHORE_READ_VALUE(vulkan);
FN_MOON_TIMELINE_SEMAPHORE_WRITE_VALUE(vulkan);
FN_MOON_TIMELINE_SEMAPHORE_WAIT_FOR_VALUE(vulkan);

FN_MOON_BINARY_SEMAPHORE_ASSEMBLY(vulkan);
FN_MOON_BINARY_SEMAPHORE_ZERO_REFCNT(vulkan);

FN_MOON_EVENT_ASSEMBLY(vulkan);
FN_MOON_EVENT_ZERO_REFCNT(vulkan);

FN_MOON_COMPUTE_PIPELINE_ASSEMBLY(vulkan);
FN_MOON_COMPUTE_PIPELINE_ZERO_REFCNT(vulkan);

FN_MOON_WORK_GRAPH_PIPELINE_ASSEMBLY(vulkan);
FN_MOON_WORK_GRAPH_PIPELINE_ZERO_REFCNT(vulkan);
FN_MOON_WORK_GRAPH_PIPELINE_NODE_INDEX(vulkan);
FN_MOON_WORK_GRAPH_PIPELINE_SCRATCH_SIZE(vulkan);

FN_MOON_RAY_TRACING_PIPELINE_ASSEMBLY(vulkan);
FN_MOON_RAY_TRACING_PIPELINE_ZERO_REFCNT(vulkan);
FN_MOON_RAY_TRACING_PIPELINE_CREATE_DEFAULT_SBT(vulkan);
FN_MOON_RAY_TRACING_PIPELINE_SHADER_GROUP_HANDLES(vulkan);

FN_MOON_RASTER_PIPELINE_ASSEMBLY(vulkan);
FN_MOON_RASTER_PIPELINE_ZERO_REFCNT(vulkan);

FN_MOON_SWAPCHAIN_ASSEMBLY(vulkan);
FN_MOON_SWAPCHAIN_ZERO_REFCNT(vulkan);
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
FN_MOON_COMMAND_RECORDER_ZERO_REFCNT(vulkan);

FN_MOON_STAGED_COMMAND_LIST_ASSEMBLY(vulkan);
FN_MOON_STAGED_COMMAND_LIST_ZERO_REFCNT(vulkan);

FN_MOON_CMD_COPY_BUFFER(vulkan);
FN_MOON_CMD_COPY_BUFFER_TO_TEXTURE(vulkan);
FN_MOON_CMD_COPY_TEXTURE_TO_BUFFER(vulkan);
FN_MOON_CMD_COPY_TEXTURE(vulkan);
FN_MOON_CMD_BLIT_TEXTURE(vulkan);
FN_MOON_CMD_RESOLVE_TEXTURE(vulkan);
FN_MOON_CMD_CLEAR_BUFFER(vulkan);
FN_MOON_CMD_CLEAR_TEXTURE(vulkan);
FN_MOON_CMD_BUILD_ACCELERATION_STRUCTURES(vulkan);
FN_MOON_CMD_DESTROY_BUFFER_DEFERRED(vulkan);
FN_MOON_CMD_DESTROY_TEXTURE_DEFERRED(vulkan);
FN_MOON_CMD_DESTROY_TEXTURE_VIEW_DEFERRED(vulkan);
FN_MOON_CMD_DESTROY_SAMPLER_DEFERRED(vulkan);
FN_MOON_CMD_PUSH_CONSTANTS(vulkan);
FN_MOON_CMD_SET_COMPUTE_PIPELINE(vulkan);
FN_MOON_CMD_SET_WORK_GRAPH_PIPELINE(vulkan);
FN_MOON_CMD_SET_RAY_TRACING_PIPELINE(vulkan);
FN_MOON_CMD_SET_RASTER_PIPELINE(vulkan);
FN_MOON_CMD_SET_VIEWPORT(vulkan);
FN_MOON_CMD_SET_SCISSOR(vulkan);
FN_MOON_CMD_SET_DEPTH_BIAS(vulkan);
FN_MOON_CMD_SET_INDEX_BUFFER(vulkan);
FN_MOON_CMD_SET_RASTERIZATION_SAMPLES(vulkan);
FN_MOON_CMD_BEGIN_RENDERPASS(vulkan);
FN_MOON_CMD_END_RENDERPASS(vulkan);
FN_MOON_CMD_WRITE_TIMESTAMPS(vulkan);
FN_MOON_CMD_BARRIERS_AND_TRANSITIONS(vulkan);
FN_MOON_CMD_RESOLVE_TIMESTAMPS(vulkan);
FN_MOON_CMD_SIGNAL_EVENT(vulkan);
FN_MOON_CMD_RESET_EVENT(vulkan);
FN_MOON_CMD_WAIT_EVENTS(vulkan);
FN_MOON_CMD_BEGIN_LABEL(vulkan);
FN_MOON_CMD_END_LABEL(vulkan);
FN_MOON_CMD_DISPATCH(vulkan);
FN_MOON_CMD_DISPATCH_INDIRECT(vulkan);
FN_MOON_CMD_DISPATCH_GRAPH(vulkan);
FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT(vulkan);
FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT_COUNT(vulkan);
FN_MOON_CMD_DISPATCH_GRAPH_SCRATCH_MEMORY(vulkan);
FN_MOON_CMD_TRACE_RAYS(vulkan);
FN_MOON_CMD_TRACE_RAYS_INDIRECT(vulkan);
FN_MOON_CMD_DRAW(vulkan);
FN_MOON_CMD_DRAW_INDEXED(vulkan);
FN_MOON_CMD_DRAW_INDIRECT(vulkan);
FN_MOON_CMD_DRAW_INDIRECT_COUNT(vulkan);
FN_MOON_CMD_DRAW_MESH_TASKS(vulkan);
FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT(vulkan);
FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT(vulkan);

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
    s8                  queue_idx;
    u32                 vk_queue_family_idx;
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
    VkBool32 has_swapchain, has_conservative_rasterization; 
    VkBool32 has_video_decode_queue, has_video_encode_queue;
};

struct physical_device {
    moon_device_details                     details;
    VkPhysicalDevice                        vk_physical_device;
    u64                                     extension_bits;

    struct queue_family                     queue_families[moon_queue_type_count];
    u32                                     unique_queue_family_indices[moon_queue_type_count];
    u32                                     unique_queue_family_count;
    u32                                     unique_queue_family_mask; /* moon_queue_mask */
    VkQueueFlags                            main_queue_command_support;

    struct physical_device_properties       vk_properties;
    struct physical_device_features         vk_features;
    struct physical_device_video_av1        vk_video_av1;
    struct physical_device_video_h264       vk_video_h264;
};

struct memory_heap_zombie { 
    VmaAllocation                           vma_allocation;
};

struct timeline_query_pool_zombie { 
    VkQueryPool                             vk_query_pool;
};

struct semaphore_zombie { 
    VkSemaphore                             vk_semaphore;
};

struct event_zombie {
    VkEvent                                 vk_event;
};

struct pipeline_zombie { 
    VkPipeline                              vk_pipeline;
};

struct command_recorder_zombie { 
    moon_queue_type                         queue_type;
    VkCommandPool                           vk_cmd_pool;
    lake_darray                             allocated_command_buffers;  /**< darray<VkCommandBuffer> */
};

struct submit_zombie {
    lake_darray                             binary_semaphores;          /**< darray<moon_binary_semaphore> */
    lake_darray                             timeline_semaphores;        /**< darray<moon_timeline_semaphore> */
};

struct buffer_impl_slot {
    moon_buffer_assembly                    assembly;
    VkBuffer                                vk_buffer;
    VmaAllocation                           vma_allocation;
    struct moon_memory_heap_impl           *optional_heap;
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
    struct moon_memory_heap_impl           *optional_heap;
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

static constexpr usize GPU_SR_POOL_MAX_RESOURCE_COUNT = (1u << 20u);
static constexpr usize GPU_SR_POOL_PAGE_BITS = 10u;
static constexpr usize GPU_SR_POOL_PAGE_SIZE = (1u << GPU_SR_POOL_PAGE_BITS);
static constexpr usize GPU_SR_POOL_PAGE_MASK = (GPU_SR_POOL_PAGE_SIZE - 1u);
static constexpr usize GPU_SR_POOL_PAGE_COUNT = (GPU_SR_POOL_MAX_RESOURCE_COUNT / GPU_SR_POOL_PAGE_SIZE);
static constexpr usize GPU_SR_POOL_VERSION_ZOMBIE_BIT = (1ull << 63ull);
static constexpr usize GPU_SR_POOL_VERSION_COUNT_MASK = ~(GPU_SR_POOL_VERSION_ZOMBIE_BIT);
#define GPU_SR_POOL_TEMPLATE(T)                                                 \
    struct T##_impl_slot_page {                                                 \
        struct T##_impl_slot        slots[GPU_SR_POOL_PAGE_SIZE];               \
        atomic_u64                  version_and_refcnt[GPU_SR_POOL_PAGE_SIZE];  \
    };                                                                          \
    struct T##_gpu_sr_pool {                                                    \
        struct T##_impl_slot_page  *pages[GPU_SR_POOL_PAGE_COUNT];              \
        lake_mpmc                   free_indices; /**< mpmc<lake_mpmc_node> */  \
        atomic_ssize                lifetime_sync;                              \
        atomic_ssize                next_idx;                                   \
        atomic_ssize                valid_page_count;                           \
        s32                         max_resources;                              \
        s32                         page_alloc_size;                            \
        lake_spinlock               page_alloc_lock;                            \
    };                                                                          \
    /** Get version of resource using an index. */                              \
    LAKE_NONNULL_ALL LAKE_HOT_FN LAKE_PURE_FN                                   \
    extern u64 LAKECALL T##_gpu_sr_pool__version_of_slot(                       \
            struct T##_gpu_sr_pool *pool,                                       \
            s32                     idx);                                       \
                                                                                \
    /** Checks if an id refers to a valid resource. */                          \
    LAKE_NONNULL_ALL LAKE_HOT_FN LAKE_PURE_FN                                   \
    extern bool LAKECALL T##_gpu_sr_pool__is_id_valid(                          \
            struct T##_gpu_sr_pool *pool,                                       \
            moon_##T##_id           id);                                        \
                                                                                \
    /** Checks if an id refers to a valid resource, may return a                \
     *  random slot if the id is invalid. */                                    \
    LAKE_NONNULL_ALL LAKE_HOT_FN LAKE_PURE_FN                                   \
    extern struct T##_impl_slot *LAKECALL T##_gpu_sr_pool__unsafe_get(          \
            struct T##_gpu_sr_pool *pool,                                       \
            moon_##T##_id           id);                                        \
                                                                                \
    /** Destroys a slot. After calling this, the id of the slot will be         \
     *  forever invalid. Index may be recycled, but index + version pairs       \
     *  are ALWAYS unique. Calling this with a non-zombie ID will result        \
     *  in undefined behaviour. */                                              \
    extern void LAKECALL T##_gpu_sr_pool__unsafe_destroy_zombie_slot(           \
            struct T##_gpu_sr_pool *pool,                                       \
            moon_##T##_id           id);                                        \
                                                                                \
    /** Try to zombify a resource. */                                           \
    extern bool LAKECALL T##_gpu_sr_pool__try_zombify(                          \
            struct T##_gpu_sr_pool *pool,                                       \
            moon_##T##_id           id);                                        \
                                                                                \
    /** Create a slot for a resource in the pool. Returned slots may be         \
     *  recycled but are guaranteed to have an unique index + version. */       \
    extern struct T##_impl_slot *LAKECALL T##_gpu_sr_pool__try_create_slot(     \
            struct T##_gpu_sr_pool *pool,                                       \
            moon_##T##_id          *out_id);
GPU_SR_POOL_TEMPLATE(buffer)
GPU_SR_POOL_TEMPLATE(texture)
GPU_SR_POOL_TEMPLATE(sampler)
GPU_SR_POOL_TEMPLATE(tlas)
GPU_SR_POOL_TEMPLATE(blas)

struct gpu_shader_resource_table {
    struct buffer_gpu_sr_pool               buffer_slots;
    struct texture_gpu_sr_pool              texture_slots;
    struct sampler_gpu_sr_pool              sampler_slots;
    struct tlas_gpu_sr_pool                 tlas_slots;
    struct blas_gpu_sr_pool                 blas_slots;
    lake_spinlock                           lifetime_lock;

    VkDescriptorSetLayout                   vk_descriptor_set_layout;
    VkDescriptorSet                         vk_descriptor_set;
    VkDescriptorPool                        vk_descriptor_pool;
    /** Contains pipeline layouts with varying push constant range size.
     *  The first size is 0 word, second is 1 word, all others are a power of 2. */
    VkPipelineLayout                        pipeline_layouts[MOON_PIPELINE_LAYOUT_COUNT];
};

struct moon_memory_heap_impl {
    moon_memory_heap_header                 header;
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

struct moon_event_impl {
    moon_event_header                       header;
    VkEvent                                 vk_event;
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
    /** darray<VkPresentModeKHR>, abstracted presentation modes, acquired from the Vulkan surface. */
    lake_darray                             supported_present_modes;
    /** darray<moon_texture_id>, swapchain holds strong references to these images, as it owns them. */
    lake_darray                             images;
    /** darray<struct moon_binary_semaphore_impl *>, signaled in the last submission that uses the swapchain image. */
    lake_darray                             present_semaphores;
    /** darray<struct moon_binary_semaphore_impl *>, signaled when the swapchain image is ready to be used. */
    lake_darray                             acquire_semaphores;
    /** cpu_timeline % frames_in_flight, used to access the acquire semaphore. */
    usize                                   acquire_semaphore_idx;
    /** Monotonically increasing timeline value. */
    u64                                     cpu_timeline;
    /** Timeline semaphore used to track how far behind the GPU is. */
    struct moon_timeline_semaphore_impl    *gpu_timeline;
    /** This presentation mode will be used if V-SYNC is disabled. */
    VkPresentModeKHR                        no_vsync_present_mode;
    /** This is the swapchain image index that acquire returns. This is not necessarily linear.
     *  This index must be used for present semaphores, as they are paired to the images. */
    u32                                     current_image_idx;
};

static constexpr s8 DEFERRED_DESTRUCTOR_BUFFER_IDX = 0;
static constexpr s8 DEFERRED_DESTRUCTOR_TEXTURE_IDX = 1;
static constexpr s8 DEFERRED_DESTRUCTOR_TEXTURE_VIEW_IDX = 2;
static constexpr s8 DEFERRED_DESTRUCTOR_SAMPLER_IDX = 3;
static constexpr s8 DEFERRED_DESTRUCTOR_TIMELINE_QUERY_POOL_IDX = 4;
static constexpr usize COMMAND_LIST_BARRIER_MAX_BATCH_SIZE = 16;
static constexpr usize COMMAND_LIST_COLOR_ATTACHMENT_MAX = 16;

struct command_pool_arena {
    lake_darray                         pools_and_buffers;      /**< darray<VkCommandPool> */
    s32                                 queue_family_idx;
    lake_spinlock                       spinlock;
};

typedef lake_pair(u64, u8) staged_deferred_destructor_pair;
struct staged_command_list_data {
    VkCommandBuffer                     vk_cmd_buffer;
    lake_darray                         deferred_destructors;   /**< darray<staged_deferred_destructor_pair> */
    lake_darray                         used_buffers;           /**< darray<moon_buffer_id> */
    lake_darray                         used_textures;          /**< darray<moon_texture_id> */
    lake_darray                         used_texture_views;     /**< darray<moon_texture_view_id> */
    lake_darray                         used_samplers;          /**< darray<moon_sampler_id> */
    lake_darray                         used_tlass;             /**< darray<moon_tlas_id> */
    lake_darray                         used_blass;             /**< darray<moon_blas_id> */
};

struct moon_command_recorder_impl {
    moon_command_recorder_header        header;
    VkCommandPool                       vk_cmd_pool;
    lake_darray                         allocated_command_buffers; /**< darray<VkCommandBuffer> */
    u32                                 memory_barrier_batch_count;
    VkMemoryBarrier2                    memory_barrier_batch[COMMAND_LIST_BARRIER_MAX_BATCH_SIZE];
    u32                                 buffer_barrier_batch_count;
    VkBufferMemoryBarrier2              buffer_barrier_batch[COMMAND_LIST_BARRIER_MAX_BATCH_SIZE];
    u32                                 image_barrier_batch_count;
    VkImageMemoryBarrier2               image_barrier_batch[COMMAND_LIST_BARRIER_MAX_BATCH_SIZE];
    u32                                 split_barrier_batch_count;
    bool                                in_renderpass;
    bool                                in_video_coding;
    moon_queue_type                     queue_type;
    enum : s8 {
        COMMAND_RECORDER_NO_PIPELINE = 0,
        COMMAND_RECORDER_COMPUTE_PIPELINE,
        COMMAND_RECORDER_WORK_GRAPH_PIPELINE,
        COMMAND_RECORDER_RASTER_PIPELINE,
        COMMAND_RECORDER_RAY_TRACING_PIPELINE,
    } current_pipeline_variant;
    union {
        struct {}                              *no_pipeline;
        struct moon_compute_pipeline_impl      *compute;
        struct moon_work_graph_pipeline_impl   *work_graph;
        struct moon_ray_tracing_pipeline_impl  *ray_tracing;
        struct moon_raster_pipeline_impl       *raster;
    } current_pipeline;
    struct staged_command_list_data     current_command_data;
};

struct moon_staged_command_list_impl {
    moon_staged_command_list_header     header;
    struct staged_command_list_data     data;
};

struct moon_impl {
    struct moon_interface_impl              interface;
    /** An array of physical device details, queried once at initialization. */
    lake_darray                             physical_devices; /**< darray<struct physical_device> */
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
    PFN_vkCreateDebugUtilsMessengerEXT                          vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT                         vkDestroyDebugUtilsMessengerEXT;
    PFN_vkSubmitDebugUtilsMessageEXT                            vkSubmitDebugUtilsMessageEXT;

    /* video coding */
    PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR                 vkGetPhysicalDeviceVideoCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR             vkGetPhysicalDeviceVideoFormatPropertiesKHR;
    PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR;
};

typedef lake_pair(u64, moon_buffer_id)                          zombie_timeline_buffer;
typedef lake_pair(u64, moon_texture_id)                         zombie_timeline_texture;
typedef lake_pair(u64, moon_texture_view_id)                    zombie_timeline_texture_view;
typedef lake_pair(u64, moon_sampler_id)                         zombie_timeline_sampler;
typedef lake_pair(u64, moon_tlas_id)                            zombie_timeline_tlas;
typedef lake_pair(u64, moon_blas_id)                            zombie_timeline_blas;
typedef lake_pair(u64, struct event_zombie)                     zombie_timeline_event;
typedef lake_pair(u64, struct pipeline_zombie)                  zombie_timeline_pipeline;
typedef lake_pair(u64, struct semaphore_zombie)                 zombie_timeline_semaphore;
typedef lake_pair(u64, struct timeline_query_pool_zombie)       zombie_timeline_query_pool;
typedef lake_pair(u64, struct command_recorder_zombie)          zombie_timeline_command_recorder;
typedef lake_pair(u64, struct memory_heap_zombie)               zombie_timeline_memory_heap;

enum : s32 {
    zombie_timeline_buffer_idx = 0,
    zombie_timeline_texture_idx,
    zombie_timeline_texture_view_idx,
    zombie_timeline_sampler_idx,
    zombie_timeline_tlas_idx,
    zombie_timeline_blas_idx,
    zombie_timeline_event_idx,
    zombie_timeline_pipeline_idx,
    zombie_timeline_semaphore_idx,
    zombie_timeline_query_pool_idx,
    zombie_timeline_command_recorder_idx,
    zombie_timeline_memory_heap_idx,
    zombie_timeline_count,
};

struct moon_device_impl {
    moon_device_header                      header;
    /** The Vulkan context of a rendering device, using the given physical device. */
    VkDevice                                vk_device;
    /** The details of the physical device used to construct this rendering device. */
    struct physical_device const           *physical_device;
    /** Points into the allocation callbacks in the adapter structure. */
    VkAllocationCallbacks const            *vk_allocator;
    /** GPU allocator from the VulkanMemoryAllocator library. */
    VmaAllocator                            vma_allocator;
    
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
    VkBuffer                                vk_null_buffer;
    VkImage                                 vk_null_image;
    VkImageView                             vk_null_image_view;
    VkSampler                               vk_null_sampler;
    VmaAllocation                           vk_null_buffer_allocation;
    VmaAllocation                           vk_null_image_allocation;

    /** Every submit to any queue increments the global submit timeline.
     *  Each queue stores a mapping between local submit index and global submit index for each 
     *  of their in flight submits. When destroying a resource it becomes a zombie, the zombie 
     *  remembers the current global timeline value. When deferred disassembly is called, the 
     *  zombies timeline values are compared against submits running in all queues. If the 
     *  zombies global submit index is smaller than the global index of all submits currently 
     *  in flight (on all queues), we can safely clean the resource up. */
    atomic_u64                              submit_timeline;
    lake_spinlock                           zombies_locks[zombie_timeline_count];
    /** Pairs of resource handles or zombies with submit timeline values. */
    lake_deque                              buffer_zombies;             /**< deque<zombie_timeline_buffer> */
    lake_deque                              texture_zombies;            /**< deque<zombie_timeline_texture> */
    lake_deque                              texture_view_zombies;       /**< deque<zombie_timeline_texture_view> */
    lake_deque                              sampler_zombies;            /**< deque<zombie_timeline_sampler> */
    lake_deque                              tlas_zombies;               /**< deque<zombie_timeline_tlas> */
    lake_deque                              blas_zombies;               /**< deque<zombie_timeline_blas> */
    lake_deque                              event_zombies;              /**< deque<zombie_timeline_event> */
    lake_deque                              pipeline_zombies;           /**< deque<zombie_timeline_pipeline> */
    lake_deque                              semaphore_zombies;          /**< deque<zombie_timeline_semaphore> */
    lake_deque                              query_pool_zombies;         /**< deque<zombie_timeline_query_pool> */
    lake_deque                              command_recorder_zombies;   /**< deque<zombie_timeline_command_recorder> */
    lake_deque                              memory_heap_zombies;        /**< deque<zombie_timeline_memory_heap> */

    /** Command buffer/pool recycling, accessed via `moon_queue_type`. */
    struct command_pool_arena               command_pool_arenas[moon_queue_type_count];
    /** Command queues in order: main, compute (8), transfer (2), sparse binding, video (2). */
    struct queue_impl                       queues[MOON_QUEUE_INDEX_COUNT];
    /** Table of GPU resource objects. */
    struct gpu_shader_resource_table        gpu_sr_table;

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

    /* debug utils */
    PFN_vkSetDebugUtilsObjectNameEXT                            vkSetDebugUtilsObjectNameEXT;
    PFN_vkSetDebugUtilsObjectTagEXT                             vkSetDebugUtilsObjectTagEXT;
    PFN_vkQueueBeginDebugUtilsLabelEXT                          vkQueueBeginDebugUtilsLabelEXT;
    PFN_vkQueueEndDebugUtilsLabelEXT                            vkQueueEndDebugUtilsLabelEXT;
    PFN_vkQueueInsertDebugUtilsLabelEXT                         vkQueueInsertDebugUtilsLabelEXT;
    PFN_vkCmdBeginDebugUtilsLabelEXT                            vkCmdBeginDebugUtilsLabelEXT;
    PFN_vkCmdEndDebugUtilsLabelEXT                              vkCmdEndDebugUtilsLabelEXT;
    PFN_vkCmdInsertDebugUtilsLabelEXT                           vkCmdInsertDebugUtilsLabelEXT;
};

static constexpr u32 DEVICE_QUEUE_IMPL_OFFSETS[moon_queue_type_count] = {
    MOON_QUEUE_MAIN_BEGIN_INDEX,
    MOON_QUEUE_COMPUTE_BEGIN_INDEX,
    MOON_QUEUE_TRANSFER_BEGIN_INDEX,
    MOON_QUEUE_SPARSE_BINDING_BEGIN_INDEX,
    MOON_QUEUE_VIDEO_DECODE_BEGIN_INDEX,
    MOON_QUEUE_VIDEO_ENCODE_BEGIN_INDEX,
};

/** Returns a message for a Vulkan result code. */
extern LAKE_CONST_FN char const *LAKECALL 
vk_result_string(
    VkResult result);

extern LAKE_CONST_FN LAKE_HOT_FN lake_result LAKECALL 
vk_result_translate(
    VkResult result);

#ifndef LAKE_NDEBUG
    #define VERIFY_VK_ERROR(x) do { \
        VkResult res__ = (x); \
        lake_dbg_assert(res__ == VK_SUCCESS, LAKE_VALIDATION_FAILED, "Vulkan API call assertion for `%s` failed, with status: %s.", #x, vk_result_string(res__)); \
    } while(0)
#else
    #define VERIFY_VK_ERROR(x) (void)(x)
#endif

LAKE_FORCE_INLINE bool is_device_queue_valid(struct moon_device_impl const *device, moon_queue queue) 
{ return queue.type < moon_queue_type_count && queue.idx < device->physical_device->queue_families[queue.type].queue_count; }

LAKE_FORCE_INLINE struct queue_impl *get_device_queue_impl(struct moon_device_impl *device, moon_queue queue)
{ return &device->queues[DEVICE_QUEUE_IMPL_OFFSETS[queue.type] + queue.idx]; }

/** A helper defined in `vk_drivers.c` to create the VkDevice using details from a physical device. */
extern VkResult LAKECALL create_vk_device_from_physical_device(
    struct moon_device_impl        *device, 
    struct physical_device const   *pd, 
    moon_explicit_features          explicit_features);

/** A helper defined in `vk_gpu_resources.c` to fill a texture slot using a swapchain image. */
extern lake_result LAKECALL create_texture_from_swapchain_image(
    struct moon_device_impl        *device, 
    VkImage                         swapchain_image, 
    VkFormat                        format,
    u32                             index,
    moon_texture_usage              usage,
    moon_texture_assembly const    *assembly,
    moon_texture_id                *out_texture);

extern lake_result LAKECALL init_gpu_sr_table(struct moon_device_impl *device);
extern void LAKECALL fini_gpu_sr_table(struct moon_device_impl *device);

extern void LAKECALL write_descriptor_set_buffer(
    struct moon_device_impl        *device,
    VkDescriptorSet                 vk_desc_set,
    VkBuffer                        vk_buffer,
    VkDeviceSize                    offset,
    VkDeviceSize                    range,
    u32                             idx);

extern void LAKECALL write_descriptor_set_image(
    struct moon_device_impl        *device,
    VkDescriptorSet                 vk_desc_set,
    VkImageView                     vk_image_view,
    moon_texture_usage              usage,
    u32                             idx);

extern void LAKECALL write_descriptor_set_sampler(
    struct moon_device_impl        *device,
    VkDescriptorSet                 vk_desc_set,
    VkSampler                       vk_sampler,
    u32                             idx);

extern void LAKECALL write_descriptor_set_acceleration_structure(
    struct moon_device_impl        *device,
    VkDescriptorSet                 vk_desc_set,
    VkAccelerationStructureKHR      vk_acceleration_structure,
    u32                             idx);

LAKE_FORCE_INLINE struct buffer_impl_slot const *acquire_buffer_slot(struct moon_device_impl *device, moon_buffer_id id)
{ return buffer_gpu_sr_pool__unsafe_get(&device->gpu_sr_table.buffer_slots, id); }

LAKE_FORCE_INLINE struct texture_impl_slot const *acquire_texture_slot(struct moon_device_impl *device, moon_texture_id id)
{ return texture_gpu_sr_pool__unsafe_get(&device->gpu_sr_table.texture_slots, id); }

LAKE_FORCE_INLINE struct texture_view_impl_slot const *acquire_texture_view_slot(struct moon_device_impl *device, moon_texture_view_id id)
{ return &texture_gpu_sr_pool__unsafe_get(&device->gpu_sr_table.texture_slots, (moon_texture_id){ .handle = id.handle })->view_slot; }

LAKE_FORCE_INLINE struct sampler_impl_slot const *acquire_sampler_slot(struct moon_device_impl *device, moon_sampler_id id)
{ return sampler_gpu_sr_pool__unsafe_get(&device->gpu_sr_table.sampler_slots, id); }

LAKE_FORCE_INLINE struct tlas_impl_slot const *acquire_tlas_slot(struct moon_device_impl *device, moon_tlas_id id)
{ return tlas_gpu_sr_pool__unsafe_get(&device->gpu_sr_table.tlas_slots, id); }

LAKE_FORCE_INLINE struct blas_impl_slot const *acquire_blas_slot(struct moon_device_impl *device, moon_blas_id id)
{ return blas_gpu_sr_pool__unsafe_get(&device->gpu_sr_table.blas_slots, id); }

LAKE_FORCE_INLINE void zombify_buffer(struct moon_device_impl *device, moon_buffer_id buffer)
{
    struct buffer_impl_slot const *slot = acquire_buffer_slot(device, buffer);
    if (slot->optional_heap != nullptr)
        moon_memory_heap_unref_v(slot->optional_heap);

    zombie_timeline_buffer submit = { 
        .first = lake_atomic_read(&device->submit_timeline), 
        .second = buffer 
    };
    lake_spinlock *lock = &device->zombies_locks[zombie_timeline_buffer_idx];
    lake_deque_unshift_w_spinlock(&device->buffer_zombies, zombie_timeline_buffer, &submit, lock, __lake_malloc, __lake_free);
}

LAKE_FORCE_INLINE void zombify_texture(struct moon_device_impl *device, moon_texture_id texture)
{
    struct texture_impl_slot const *slot = acquire_texture_slot(device, texture);
    if (slot->optional_heap != nullptr)
        moon_memory_heap_unref_v(slot->optional_heap);

    zombie_timeline_texture submit = { 
        .first = lake_atomic_read(&device->submit_timeline), 
        .second = texture
    };
    lake_spinlock *lock = &device->zombies_locks[zombie_timeline_texture_idx];
    lake_deque_unshift_w_spinlock(&device->texture_zombies, zombie_timeline_texture, &submit, lock, __lake_malloc, __lake_free);
}

LAKE_FORCE_INLINE void zombify_texture_view(struct moon_device_impl *device, moon_texture_view_id texture_view)
{
    zombie_timeline_texture_view submit = { 
        .first = lake_atomic_read(&device->submit_timeline), 
        .second = texture_view
    };
    lake_spinlock *lock = &device->zombies_locks[zombie_timeline_texture_view_idx];
    lake_deque_unshift_w_spinlock(&device->texture_view_zombies, zombie_timeline_texture_view, &submit, lock, __lake_malloc, __lake_free);
}

LAKE_FORCE_INLINE void zombify_sampler(struct moon_device_impl *device, moon_sampler_id sampler)
{
    zombie_timeline_sampler submit = { 
        .first = lake_atomic_read(&device->submit_timeline), 
        .second = sampler
    };
    lake_spinlock *lock = &device->zombies_locks[zombie_timeline_sampler_idx];
    lake_deque_unshift_w_spinlock(&device->sampler_zombies, zombie_timeline_sampler, &submit, lock, __lake_malloc, __lake_free);
}

LAKE_FORCE_INLINE void zombify_tlas(struct moon_device_impl *device, moon_tlas_id tlas)
{
    struct tlas_impl_slot const *slot = acquire_tlas_slot(device, tlas);
    if (slot->owns_buffer) {
        zombify_buffer(device, slot->buffer_id);
    }
    zombie_timeline_tlas submit = { 
        .first = lake_atomic_read(&device->submit_timeline), 
        .second = tlas 
    };
    lake_spinlock *lock = &device->zombies_locks[zombie_timeline_tlas_idx];
    lake_deque_unshift_w_spinlock(&device->tlas_zombies, zombie_timeline_tlas, &submit, lock, lake_machina);
}

LAKE_FORCE_INLINE void zombify_blas(struct moon_device_impl *device, moon_blas_id blas)
{
    struct blas_impl_slot const *slot = acquire_blas_slot(device, blas);
    if (slot->owns_buffer) {
        zombify_buffer(device, slot->buffer_id);
    }
    zombie_timeline_blas submit = { 
        .first = lake_atomic_read(&device->submit_timeline), 
        .second = blas 
    };
    lake_spinlock *lock = &device->zombies_locks[zombie_timeline_blas_idx];
    lake_deque_unshift_w_spinlock(&device->blas_zombies, zombie_timeline_blas, &submit, lock, __lake_malloc, __lake_free);
}

LAKE_FORCE_INLINE void buffer_destructor(struct moon_device_impl *device, moon_buffer_id buffer)
{
    struct buffer_impl_slot const *slot = acquire_buffer_slot(device, buffer);
    /* Does not need external sync given we use update after bind.
     * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDescriptorBindingFlagBits.html */
    write_descriptor_set_buffer(device, device->gpu_sr_table.vk_descriptor_set, device->vk_null_buffer, 0, VK_WHOLE_SIZE, moon_id_get_index(buffer));
    if (slot->optional_heap != nullptr) {
        device->vkDestroyBuffer(device->vk_device, slot->vk_buffer, device->vk_allocator);
    } else {
        vmaDestroyBuffer(device->vma_allocator, slot->vk_buffer, slot->vma_allocation);
    }
    buffer_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.buffer_slots, buffer);
}

LAKE_FORCE_INLINE void texture_destructor(struct moon_device_impl *device, moon_texture_id texture)
{
    struct texture_impl_slot const *slot = acquire_texture_slot(device, texture);
    /* Does not need external sync given we use update after bind.
     * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDescriptorBindingFlagBits.html */
    write_descriptor_set_image(device, device->gpu_sr_table.vk_descriptor_set, device->vk_null_image_view, slot->assembly.usage, moon_id_get_index(texture));
    if (slot->view_slot.vk_image_view != nullptr)
        device->vkDestroyImageView(device->vk_device, slot->view_slot.vk_image_view, device->vk_allocator);
    if (slot->swapchain_image_idx == NOT_OWNED_BY_SWAPCHAIN) {
        if (slot->optional_heap != nullptr) {
            device->vkDestroyImage(device->vk_device, slot->vk_image, device->vk_allocator);
        } else {
            vmaDestroyImage(device->vma_allocator, slot->vk_image, slot->vma_allocation);
        }
    }
    texture_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.texture_slots, texture);
}

LAKE_FORCE_INLINE void texture_view_destructor(struct moon_device_impl *device, moon_texture_view_id texture_view)
{
    struct texture_view_impl_slot const *slot = acquire_texture_view_slot(device, texture_view);
    /* Does not need external sync given we use update after bind.
     * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDescriptorBindingFlagBits.html */
    write_descriptor_set_image(device, device->gpu_sr_table.vk_descriptor_set, device->vk_null_image_view, 
        moon_texture_usage_shader_storage | moon_texture_usage_shader_sampled, moon_id_get_index(texture_view));
    device->vkDestroyImageView(device->vk_device, slot->vk_image_view, device->vk_allocator);
    texture_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.texture_slots, moon_id_t(moon_texture_id, texture_view));
}

LAKE_FORCE_INLINE void sampler_destructor(struct moon_device_impl *device, moon_sampler_id sampler)
{
    struct sampler_impl_slot const *slot = acquire_sampler_slot(device, sampler);
    /* Does not need external sync given we use update after bind.
     * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDescriptorBindingFlagBits.html */
    write_descriptor_set_sampler(device, device->gpu_sr_table.vk_descriptor_set, device->vk_null_sampler, moon_id_get_index(sampler));
    device->vkDestroySampler(device->vk_device, slot->vk_sampler, device->vk_allocator);
    sampler_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.sampler_slots, sampler);
}

LAKE_FORCE_INLINE void tlas_destructor(struct moon_device_impl *device, moon_tlas_id tlas)
{
    struct tlas_impl_slot const *slot = acquire_tlas_slot(device, tlas);
    device->vkDestroyAccelerationStructureKHR(device->vk_device, slot->vk_acceleration_structure, device->vk_allocator);
    tlas_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.tlas_slots, tlas);
}

LAKE_FORCE_INLINE void blas_destructor(struct moon_device_impl *device, moon_blas_id blas)
{
    struct blas_impl_slot const *slot = acquire_blas_slot(device, blas);
    device->vkDestroyAccelerationStructureKHR(device->vk_device, slot->vk_acceleration_structure, device->vk_allocator);
    blas_gpu_sr_pool__unsafe_destroy_zombie_slot(&device->gpu_sr_table.blas_slots, blas);
}

LAKE_FORCE_INLINE VkBufferUsageFlags get_buffer_usage_flags(struct moon_device_impl *device)
{
    VkBufferUsageFlags result = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    if (device->header.details->implicit_features & moon_implicit_feature_basic_ray_tracing) {
        result |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
                  VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
                  VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;
    }
    return result;
}

extern LAKE_CONST_FN bool LAKECALL is_depth_format(moon_format format);
extern LAKE_CONST_FN bool LAKECALL is_stencil_format(moon_format format);

LAKE_FORCE_INLINE LAKE_CONST_FN VkImageAspectFlags infer_aspect_from_format(moon_format format)
{
    bool is_depth = is_depth_format(format);
    bool is_stencil = is_stencil_format(format);
    if (!is_depth && !is_stencil)
        return VK_IMAGE_ASPECT_COLOR_BIT;
    return (is_depth ? VK_IMAGE_ASPECT_DEPTH_BIT : 0) | (is_stencil ? VK_IMAGE_ASPECT_STENCIL_BIT : 0);
}

LAKE_FORCE_INLINE VkImageSubresourceRange make_subresource_range(
    moon_texture_mip_array_slice const *slice, 
    VkImageAspectFlags                  aspect)
{
    return (VkImageSubresourceRange){
        .aspectMask = aspect,
        .baseMipLevel = slice->base_mip_level,
        .levelCount = slice->level_count,
        .baseArrayLayer = slice->base_array_layer,
        .layerCount = slice->layer_count,
    };
}

LAKE_FORCE_INLINE VkImageSubresourceLayers make_subresource_layers(
    moon_texture_array_slice const *slice, 
    VkImageAspectFlags              aspect)
{
    return (VkImageSubresourceLayers){
        .aspectMask = aspect,
        .mipLevel = slice->mip_level,
        .baseArrayLayer = slice->base_array_layer,
        .layerCount = slice->layer_count,
    };
}

LAKE_FORCE_INLINE moon_texture_mip_array_slice validate_texture_slice(
    struct moon_device_impl            *device, 
    moon_texture_mip_array_slice const *slice, 
    moon_texture_view_id                texture_view)
{
    return (slice->level_count == UINT32_MAX || slice->level_count == 0)
        ? acquire_texture_view_slot(device, texture_view)->assembly.slice : *slice;
}

LAKE_HOT_FN
extern void LAKECALL populate_vk_access_info(
    u32                             access_count,
    moon_access const              *accesses,
    VkPipelineStageFlags2          *out_stage_mask,
    VkAccessFlags2                 *out_access_mask,
    VkImageLayout                  *out_image_layout,
    bool                           *has_write_access);

LAKE_NONNULL_ALL 
extern void LAKECALL populate_vk_memory_barrier(
    moon_global_barrier const      *global_barrier, 
    VkMemoryBarrier2               *out_barrier);

LAKE_NONNULL_ALL 
extern void LAKECALL populate_vk_buffer_memory_barrier(
    struct moon_device_impl        *device,
    moon_buffer_barrier const      *buffer_barrier, 
    VkBufferMemoryBarrier2         *out_barrier);

LAKE_NONNULL_ALL 
extern void LAKECALL populate_vk_image_memory_barrier(
    struct moon_device_impl        *device,
    moon_texture_barrier const     *texture_barrier, 
    VkImageMemoryBarrier2          *out_barrier);

LAKE_NONNULL_ALL 
extern void LAKECALL populate_vk_image_create_info_from_assembly(
    struct moon_device_impl        *device, 
    moon_texture_assembly const    *assembly,
    VkImageCreateInfo              *out_vk_create_info);

struct acceleratrion_structure_build {
    lake_darray                             vk_build_geometry_infos; /**< darray<VkAccelerationStructureBuildGeometryInfoKHR> */
    lake_darray                             vk_geometry_infos;       /**< darray<VkAccelerationStructureGeometryKHR> */
    lake_darray                             primitive_counts;        /**< darray<u32> */
    lake_darray                             primitive_counts_ptrs;   /**< darray<u32 const *> */
};

LAKE_NONNULL(1)
extern void LAKECALL populate_vk_acceleration_structure_build_details(
    struct moon_device_impl                *device,
    usize                                   tlas_build_count,
    moon_tlas_build_details const          *tlas_build_details,
    usize                                   blas_build_count,
    moon_blas_build_details const          *blas_build_details,
    struct acceleratrion_structure_build   *out_build);

#endif /* MOON_VULKAN */
