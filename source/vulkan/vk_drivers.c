#include "vk_moon.h"
#ifdef MOON_VULKAN

enum instance_extension_bits {
    instance_extension_khr_surface              = (1u << 0),    /**< VK_KHR_surface */
    instance_extension_khr_win32_surface        = (1u << 1),    /**< VK_KHR_win32_surface */
    instance_extension_ext_metal_surface        = (1u << 2),    /**< VK_EXT_metal_surface */
    instance_extension_khr_android_surface      = (1u << 3),    /**< VK_KHR_android_surface */
    instance_extension_khr_wayland_surface      = (1u << 4),    /**< VK_KHR_wayland_surface */
    instance_extension_khr_xcb_surface          = (1u << 5),    /**< VK_KHR_xcb_surface */
    instance_extension_khr_headless_surface     = (1u << 6),    /**< VK_KHR_headless_surface */
    instance_extension_khr_display              = (1u << 7),    /**< VK_KHR_display */
    instance_extension_ext_debug_utils          = (1u << 8),    /**< VK_EXT_debug_utils */
    instance_extension_count = 9,
    instance_extension_layer_validation         = (1u << 9),    /**< VK_LAYER_KHRONOS_validation */
};

static char const *g_instance_extension_names[instance_extension_count] = {
    "VK_KHR_surface",
    "VK_KHR_win32_surface",
    "VK_EXT_metal_surface",
    "VK_KHR_android_surface",
    "VK_KHR_wayland_surface",
    "VK_KHR_xcb_surface",
    "VK_KHR_headless_surface",
    "VK_KHR_display",
    "VK_EXT_debug_utils",
};

enum device_extension_bits {
    device_extension_khr_swapchain                             = (1ull << 0),  /**< VK_KHR_swapchain */
    device_extension_ext_device_fault                          = (1ull << 1),  /**< VK_EXT_device_fault */
    device_extension_ext_memory_budget                         = (1ull << 2),  /**< VK_EXT_memory_budget */
    device_extension_ext_memory_priority                       = (1ull << 3),  /**< VK_EXT_memory_priority */
    device_extension_ext_mesh_shader                           = (1ull << 4),  /**< VK_EXT_mesh_shader */
    device_extension_khr_fragment_shading_rate                 = (1ull << 5),  /**< VK_KHR_fragment_shading_rate */
    device_extension_khr_deferred_host_operations              = (1ull << 6),  /**< VK_KHR_deferred_host_operations */
    device_extension_khr_acceleration_structure                = (1ull << 7),  /**< VK_KHR_acceleration_structure */
    device_extension_khr_pipeline_library                      = (1ull << 8),  /**< VK_KHR_pipeline_library */
    device_extension_khr_ray_query                             = (1ull << 9),  /**< VK_KHR_ray_query */
    device_extension_khr_ray_tracing_pipeline                  = (1ull << 10), /**< VK_KHR_ray_tracing_pipeline */
    device_extension_khr_ray_tracing_maintenance1              = (1ull << 11), /**< VK_KHR_ray_tracing_maintenance1 */
    device_extension_khr_ray_tracing_position_fetch            = (1ull << 12), /**< VK_KHR_ray_tracing_position_fetch */
    device_extension_khr_video_queue                           = (1ull << 13), /**< VK_KHR_video_queue */
    device_extension_khr_video_decode_queue                    = (1ull << 14), /**< VK_KHR_video_decode_queue */
    device_extension_khr_video_decode_av1                      = (1ull << 15), /**< VK_KHR_video_decode_av1 */
    device_extension_khr_video_decode_h264                     = (1ull << 16), /**< VK_KHR_video_decode_h264 */
    device_extension_khr_video_encode_queue                    = (1ull << 17), /**< VK_KHR_video_encode_queue */
    device_extension_khr_video_encode_av1                      = (1ull << 18), /**< VK_KHR_video_encode_av1 */
    device_extension_khr_video_encode_h264                     = (1ull << 19), /**< VK_KHR_video_encode_h264 */
    device_extension_khr_video_maintenance1                    = (1ull << 20), /**< VK_KHR_video_maintenance1 */
    device_extension_ext_extended_dynamic_state3               = (1ull << 21), /**< VK_EXT_extended_dynamic_state3 */
    device_extension_ext_robustness2                           = (1ull << 22), /**< VK_EXT_robustness2 */
    device_extension_ext_shader_image_atomic_int64             = (1ull << 23), /**< VK_EXT_shader_image_atomic_int64 */
    device_extension_ext_shader_atomic_float                   = (1ull << 24), /**< VK_EXT_shader_atomic_float */
    device_extension_ext_conservative_rasterization            = (1ull << 25), /**< VK_EXT_conservative_rasterization */
    device_extension_ext_dynamic_rendering_unused_attachments  = (1ull << 26), /**< VK_EXT_dynamic_rendering_unused_attachments */
    /* NVIDIA hardware */
    device_extension_nv_ray_tracing_invocation_reorder         = (1ull << 27), /**< VK_NV_ray_tracing_invocation_reorder */
    /* AMD hardware */
    device_extension_amd_device_coherent_memory                = (1ull << 28), /**< VK_AMD_device_coherent_memory */
    device_extension_amdx_shader_enqueue                       = (1ull << 29), /**< VK_AMDX_shader_enqueue, work graph */
    device_extension_count = 30,
    /* core 1.4, for backwards compatibility */
    device_extension_khr_dynamic_rendering_local_read          = (1ull << 30), /**< VK_KHR_dynamic_rendering_local_read */
    device_extension_khr_maintenance6                          = (1ull << 31), /**< VK_KHR_maintenance6 */
    device_extension_khr_maintenance5                          = (1ull << 32), /**< VK_KHR_maintenance5 */
    device_extension_count_1_4_fallback = 33,
    /* core 1.3, for backwards compatibility */
    device_extension_khr_dynamic_rendering                     = (1ull << 33), /**< VK_KHR_dynamic_rendering */
    device_extension_khr_synchronization2                      = (1ull << 34), /**< VK_KHR_synchronization2 */
    device_extension_khr_maintenance4                          = (1ull << 35), /**< VK_KHR_maintenance4 */
    device_extension_count_1_3_fallback = 36,
};

static char const *g_device_extension_names[device_extension_count_1_3_fallback] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_DEVICE_FAULT_EXTENSION_NAME,
    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
    VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME,
    VK_EXT_MESH_SHADER_EXTENSION_NAME,
    VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
    VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
    VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
    VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
    VK_KHR_RAY_QUERY_EXTENSION_NAME,
    VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
    VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME,
    VK_KHR_RAY_TRACING_POSITION_FETCH_EXTENSION_NAME,
    VK_KHR_VIDEO_QUEUE_EXTENSION_NAME,
    VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME,
    VK_KHR_VIDEO_DECODE_AV1_EXTENSION_NAME,
    VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME,
    VK_KHR_VIDEO_ENCODE_QUEUE_EXTENSION_NAME,
    VK_KHR_VIDEO_ENCODE_AV1_EXTENSION_NAME,
    VK_KHR_VIDEO_ENCODE_H264_EXTENSION_NAME,
    VK_KHR_VIDEO_MAINTENANCE_1_EXTENSION_NAME,
    VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME,
    VK_EXT_ROBUSTNESS_2_EXTENSION_NAME,
    VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME,
    VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
    VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME,
    VK_EXT_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_EXTENSION_NAME,
    VK_NV_RAY_TRACING_INVOCATION_REORDER_EXTENSION_NAME,
    VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME,
    VK_AMDX_SHADER_ENQUEUE_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_6_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_5_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
};

LAKE_CONST_FN char const *LAKECALL vk_result_string(VkResult result)
{
    switch (result) {
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			return "host memory allocation has failed";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return "device memory allocation has failed";
		case VK_ERROR_INITIALIZATION_FAILED:
			return "initialization of an object could not be completed for implementation-specific reasons";
		case VK_ERROR_DEVICE_LOST:
			return "the logical or physical device has been lost";
		case VK_ERROR_MEMORY_MAP_FAILED:
			return "mapping of a memory object has failed";
		case VK_ERROR_LAYER_NOT_PRESENT:
			return "a requested layer is not present or could not be loaded";
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			return "a requested extension is not supported";
		case VK_ERROR_FEATURE_NOT_PRESENT:
			return "a requested feature is not supported";
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			return "the requested version of Vulkan is not supported by the driver or is otherwise "
			       "incompatible for implementation-specific reasons";
		case VK_ERROR_TOO_MANY_OBJECTS:
			return "too many objects of the type have already been created";
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			return "a requested format is not supported on this device";
		case VK_ERROR_FRAGMENTED_POOL:
			return "a pool allocation has failed due to fragmentation of the pool's memory";
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			return "a pool memory allocation has failed";
		case VK_ERROR_INVALID_EXTERNAL_HANDLE:
			return "an external handle is not a valid handle of the specified type";
		case VK_ERROR_FRAGMENTATION:
			return "a descriptor pool creation has failed due to fragmentation";
		case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
			return "a buffer creation or memory allocation failed because the requested address is not available";
		case VK_PIPELINE_COMPILE_REQUIRED:
			return "a requested pipeline creation would have required compilation, but the application requested compilation to not be performed";
		case VK_ERROR_SURFACE_LOST_KHR:
			return "a surface is no longer available";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			return "the requested window is already in use by Vulkan or another API in a manner which prevents it from being used again";
		case VK_SUBOPTIMAL_KHR:
			return "a swapchain no longer matches the surface properties exactly, but can still be used to present"
			       "to the surface successfully";
		case VK_ERROR_OUT_OF_DATE_KHR:
			return "a surface has changed in such a way that it is no longer compatible with the swapchain, "
			       "any further presentation requests using the swapchain will fail";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			return "the display used by a swapchain does not use the same presentable image layout, or is "
			       "incompatible in a way that prevents sharing an image";
		case VK_ERROR_VALIDATION_FAILED_EXT:
			return "VK_ERROR_VALIDATION_FAILED_EXT";
		case VK_ERROR_INVALID_SHADER_NV:
			return "one or more shaders failed to compile or link";
		case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
			return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
		case VK_ERROR_NOT_PERMITTED_KHR:
			return "VK_ERROR_NOT_PERMITTED_KHR";
		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			return "an operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as "
			       "it did not have exlusive full-screen access";
		case VK_THREAD_IDLE_KHR:
			return "a deferred operation is not complete but there is currently no work for this thread to do at the time of this call";
		case VK_THREAD_DONE_KHR:
			return "a deferred operation is not complete but there is no work remaining to assign to additional threads";
		case VK_OPERATION_DEFERRED_KHR:
			return "a deferred operation was requested and at least some of the work was deferred";
		case VK_OPERATION_NOT_DEFERRED_KHR:
			return "a deferred operation was requested and no operations were deferred";
		case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
			return "an image creation failed because internal resources required for compression are exhausted."
			       "this must only be returned when fixed-rate compression is requested";
        case VK_INCOMPLETE:
            return "incomplete";
        case VK_SUCCESS:
            return "success";
        default:
            return "an unknown error has occured";
    }
}

static bool query_extension(
    VkExtensionProperties *properties, 
    u32 const              count, 
    char const      *const extension)
{
    for (u32 i = 0; i < count; i++)
        if (!strcmp(properties[i].extensionName, extension))
            return true;
    return false;
}

#ifndef LAKE_NDEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL
handle_debug_utils_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
    VkDebugUtilsMessageTypeFlagsEXT             type,
    const VkDebugUtilsMessengerCallbackDataEXT *callbackdata,
    void                                       *userdata)
{
    (void)userdata; (void)type;

    switch (severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            lake_trace("%s", callbackdata->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            lake_dbg_2("%s", callbackdata->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            lake_warn("%s", callbackdata->pMessage);
            break;
        default:
            lake_assert(!"[red] Vulkan validation error !!", VK_ERROR_VALIDATION_FAILED_EXT, "%s", callbackdata->pMessage);
    }
    return VK_FALSE;
}
#endif /* LAKE_NDEBUG */

static VKAPI_ATTR void *VKAPI_CALL 
handle_allocation_callback__malloc(
    void                   *userdata,
    usize                   size,
    usize                   align,
    VkSystemAllocationScope allocation_scope)
{
    (void)allocation_scope;
    (void)userdata;
    return __lake_malloc(size, align);
}

static VKAPI_ATTR void *VKAPI_CALL
handle_allocation_callback__realloc(
    void                   *userdata,
    void                   *memory,
    usize                   size,
    usize                   align,
    VkSystemAllocationScope allocation_scope)
{
    (void)allocation_scope;
    (void)userdata;
    return __lake_realloc(memory, size, align);
}

static VKAPI_ATTR void VKAPI_CALL
handle_allocation_callback__free(
    void *userdata,
    void *memory)
{
    (void)userdata;
    __lake_free(memory);
}

static PFN_vkVoidFunction get_vk_instance_proc_address(moon_adapter const moon, char const *procname)
{
    PFN_vkVoidFunction address = moon->vkGetInstanceProcAddr(moon->vk_instance, procname);
    if (address == nullptr)
        lake_warn("Can't find Vulkan instance procedure `%s`.", procname);
    return address;
}

static PFN_vkVoidFunction get_vk_device_proc_address(moon_device const device, char const *procname)
{
    PFN_vkVoidFunction address = device->header.moon.adapter->vkGetDeviceProcAddr(device->vk_device, procname);
    if (address == nullptr)
        lake_warn("Can't find Vulkan device procedure `%s`.", procname);
    return address;
}

static bool load_vk_instance_symbols(moon_adapter moon, u32 extension_bits)
{
    lake_dbg_assert(moon != nullptr && moon->vkGetInstanceProcAddr, LAKE_ERROR_INCOMPATIBLE_DRIVER, nullptr);

    moon->vkCreateDevice = (PFN_vkCreateDevice)
        get_vk_instance_proc_address(moon, "vkCreateDevice");
    moon->vkDestroyInstance = (PFN_vkDestroyInstance)
        get_vk_instance_proc_address(moon, "vkDestroyInstance");
    moon->vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)
        get_vk_instance_proc_address(moon, "vkEnumerateDeviceExtensionProperties");
    moon->vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)
        get_vk_instance_proc_address(moon, "vkEnumeratePhysicalDevices");
    moon->vkEnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups)
        get_vk_instance_proc_address(moon, "vkEnumeratePhysicalDeviceGroups");
    moon->vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)
        get_vk_instance_proc_address(moon, "vkGetDeviceProcAddr");
    moon->vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceFeatures");
    moon->vkGetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceFeatures2");
    moon->vkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceFormatProperties");
    moon->vkGetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceFormatProperties2");
    moon->vkGetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceImageFormatProperties");
    moon->vkGetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceImageFormatProperties2");
    moon->vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceMemoryProperties");
    moon->vkGetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceMemoryProperties2");
    moon->vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceProperties");
    moon->vkGetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceProperties2");
    moon->vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceQueueFamilyProperties");
    moon->vkGetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceQueueFamilyProperties2");
    moon->vkGetPhysicalDeviceSparseImageFormatProperties = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceSparseImageFormatProperties");
    moon->vkGetPhysicalDeviceSparseImageFormatProperties2 = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceSparseImageFormatProperties2");
    moon->vkGetPhysicalDeviceToolProperties = (PFN_vkGetPhysicalDeviceToolProperties)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceToolProperties");

    if (!moon->vkCreateDevice ||
        !moon->vkDestroyInstance ||
        !moon->vkEnumerateDeviceExtensionProperties ||
        !moon->vkEnumeratePhysicalDevices ||
        !moon->vkEnumeratePhysicalDeviceGroups ||
        !moon->vkGetDeviceProcAddr ||
        !moon->vkGetPhysicalDeviceFeatures ||
        !moon->vkGetPhysicalDeviceFeatures2 ||
        !moon->vkGetPhysicalDeviceFormatProperties ||
        !moon->vkGetPhysicalDeviceFormatProperties2 ||
        !moon->vkGetPhysicalDeviceImageFormatProperties ||
        !moon->vkGetPhysicalDeviceImageFormatProperties2 ||
        !moon->vkGetPhysicalDeviceMemoryProperties ||
        !moon->vkGetPhysicalDeviceMemoryProperties2 ||
        !moon->vkGetPhysicalDeviceProperties ||
        !moon->vkGetPhysicalDeviceProperties2 ||
        !moon->vkGetPhysicalDeviceQueueFamilyProperties ||
        !moon->vkGetPhysicalDeviceQueueFamilyProperties2 ||
        !moon->vkGetPhysicalDeviceSparseImageFormatProperties ||
        !moon->vkGetPhysicalDeviceSparseImageFormatProperties2 ||
        !moon->vkGetPhysicalDeviceToolProperties)
        return false;

    /* VK_KHR_surface */
    if (extension_bits & instance_extension_khr_surface) {
        moon->vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)
            get_vk_instance_proc_address(moon, "vkDestroySurfaceKHR");
        moon->vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)
            get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceSurfaceSupportKHR");
        moon->vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
            get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        moon->vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)
            get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        moon->vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)
            get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceSurfacePresentModesKHR");

        if (!moon->vkDestroySurfaceKHR ||
            !moon->vkGetPhysicalDeviceSurfaceSupportKHR ||
            !moon->vkGetPhysicalDeviceSurfaceCapabilitiesKHR ||
            !moon->vkGetPhysicalDeviceSurfaceFormatsKHR ||
            !moon->vkGetPhysicalDeviceSurfacePresentModesKHR)
            return false;
    }

    /* VK_EXT_debug_utils */
    if (extension_bits & instance_extension_ext_debug_utils) {
        moon->vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)
            get_vk_instance_proc_address(moon, "vkSetDebugUtilsObjectNameEXT");
        moon->vkSetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT)
            get_vk_instance_proc_address(moon, "vkSetDebugUtilsObjectTagEXT");
        moon->vkQueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT)
            get_vk_instance_proc_address(moon, "vkQueueBeginDebugUtilsLabelEXT");
        moon->vkQueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT)
            get_vk_instance_proc_address(moon, "vkQueueEndDebugUtilsLabelEXT");
        moon->vkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT)
            get_vk_instance_proc_address(moon, "vkQueueInsertDebugUtilsLabelEXT");
        moon->vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)
            get_vk_instance_proc_address(moon, "vkCmdBeginDebugUtilsLabelEXT");
        moon->vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)
            get_vk_instance_proc_address(moon, "vkCmdEndDebugUtilsLabelEXT");
        moon->vkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)
            get_vk_instance_proc_address(moon, "vkCmdInsertDebugUtilsLabelEXT");
        moon->vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)
            get_vk_instance_proc_address(moon, "vkCreateDebugUtilsMessengerEXT");
        moon->vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)
            get_vk_instance_proc_address(moon, "vkDestroyDebugUtilsMessengerEXT");
        moon->vkSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)
            get_vk_instance_proc_address(moon, "vkSubmitDebugUtilsMessageEXT");

        if (!moon->vkSetDebugUtilsObjectNameEXT ||
            !moon->vkSetDebugUtilsObjectTagEXT ||
            !moon->vkQueueBeginDebugUtilsLabelEXT ||
            !moon->vkQueueEndDebugUtilsLabelEXT ||
            !moon->vkQueueInsertDebugUtilsLabelEXT ||
            !moon->vkCmdBeginDebugUtilsLabelEXT ||
            !moon->vkCmdEndDebugUtilsLabelEXT ||
            !moon->vkCmdInsertDebugUtilsLabelEXT ||
            !moon->vkCreateDebugUtilsMessengerEXT ||
            !moon->vkDestroyDebugUtilsMessengerEXT ||
            !moon->vkSubmitDebugUtilsMessageEXT)
            return false;
    }

    /* VK_KHR_video_queue, optional */
    moon->vkGetPhysicalDeviceVideoCapabilitiesKHR = (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
    moon->vkGetPhysicalDeviceVideoFormatPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceVideoFormatPropertiesKHR");
    moon->vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR)
        get_vk_instance_proc_address(moon, "vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR");
    if (!moon->vkGetPhysicalDeviceVideoCapabilitiesKHR ||
        !moon->vkGetPhysicalDeviceVideoFormatPropertiesKHR ||
        !moon->vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR)
    {
        moon->vkGetPhysicalDeviceVideoCapabilitiesKHR = nullptr;
        moon->vkGetPhysicalDeviceVideoFormatPropertiesKHR  = nullptr;
        moon->vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR = nullptr;
    }
    return true;
}

static bool load_vk_device_symbols(moon_device device, u64 extension_bits)
{
    lake_dbg_assert(device != nullptr && device->header.moon.adapter != nullptr 
        && device->header.moon.adapter->vkGetDeviceProcAddr, LAKE_ERROR_INCOMPATIBLE_DRIVER, nullptr);

    /* core 1.0 */
	device->vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)
        get_vk_device_proc_address(device, "vkAllocateCommandBuffers");
	device->vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)
        get_vk_device_proc_address(device, "vkAllocateDescriptorSets");
	device->vkAllocateMemory = (PFN_vkAllocateMemory)
        get_vk_device_proc_address(device, "vkAllocateMemory");
	device->vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)
        get_vk_device_proc_address(device, "vkBeginCommandBuffer");
	device->vkBindBufferMemory = (PFN_vkBindBufferMemory)
        get_vk_device_proc_address(device, "vkBindBufferMemory");
	device->vkBindImageMemory = (PFN_vkBindImageMemory)
        get_vk_device_proc_address(device, "vkBindImageMemory");
	device->vkCmdBeginQuery = (PFN_vkCmdBeginQuery)
        get_vk_device_proc_address(device, "vkCmdBeginQuery");
	device->vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)
        get_vk_device_proc_address(device, "vkCmdBeginRenderPass");
	device->vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)
        get_vk_device_proc_address(device, "vkCmdBindDescriptorSets");
	device->vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)
        get_vk_device_proc_address(device, "vkCmdBindIndexBuffer");
	device->vkCmdBindPipeline = (PFN_vkCmdBindPipeline)
        get_vk_device_proc_address(device, "vkCmdBindPipeline");
	device->vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)
        get_vk_device_proc_address(device, "vkCmdBindVertexBuffers");
	device->vkCmdBlitImage = (PFN_vkCmdBlitImage)
        get_vk_device_proc_address(device, "vkCmdBlitImage");
	device->vkCmdClearAttachments = (PFN_vkCmdClearAttachments)
        get_vk_device_proc_address(device, "vkCmdClearAttachments");
	device->vkCmdClearColorImage = (PFN_vkCmdClearColorImage)
        get_vk_device_proc_address(device, "vkCmdClearColorImage");
	device->vkCmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage)
        get_vk_device_proc_address(device, "vkCmdClearDepthStencilImage");
	device->vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer)
        get_vk_device_proc_address(device, "vkCmdCopyBuffer");
	device->vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)
        get_vk_device_proc_address(device, "vkCmdCopyBufferToImage");
	device->vkCmdCopyImage = (PFN_vkCmdCopyImage)
        get_vk_device_proc_address(device, "vkCmdCopyImage");
	device->vkCmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)
        get_vk_device_proc_address(device, "vkCmdCopyImageToBuffer");
	device->vkCmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults)
        get_vk_device_proc_address(device, "vkCmdCopyQueryPoolResults");
	device->vkCmdDispatch = (PFN_vkCmdDispatch)
        get_vk_device_proc_address(device, "vkCmdDispatch");
	device->vkCmdDispatchIndirect = (PFN_vkCmdDispatchIndirect)
        get_vk_device_proc_address(device, "vkCmdDispatchIndirect");
	device->vkCmdDraw = (PFN_vkCmdDraw)
        get_vk_device_proc_address(device, "vkCmdDraw");
	device->vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed)
        get_vk_device_proc_address(device, "vkCmdDrawIndexed");
	device->vkCmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect)
        get_vk_device_proc_address(device, "vkCmdDrawIndexedIndirect");
	device->vkCmdDrawIndirect = (PFN_vkCmdDrawIndirect)
        get_vk_device_proc_address(device, "vkCmdDrawIndirect");
	device->vkCmdEndQuery = (PFN_vkCmdEndQuery)
        get_vk_device_proc_address(device, "vkCmdEndQuery");
	device->vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)
        get_vk_device_proc_address(device, "vkCmdEndRenderPass");
	device->vkCmdExecuteCommands = (PFN_vkCmdExecuteCommands)
        get_vk_device_proc_address(device, "vkCmdExecuteCommands");
	device->vkCmdFillBuffer = (PFN_vkCmdFillBuffer)
        get_vk_device_proc_address(device, "vkCmdFillBuffer");
	device->vkCmdNextSubpass = (PFN_vkCmdNextSubpass)
        get_vk_device_proc_address(device, "vkCmdNextSubpass");
	device->vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)
        get_vk_device_proc_address(device, "vkCmdPipelineBarrier");
	device->vkCmdPushConstants = (PFN_vkCmdPushConstants)
        get_vk_device_proc_address(device, "vkCmdPushConstants");
	device->vkCmdResetEvent = (PFN_vkCmdResetEvent)
        get_vk_device_proc_address(device, "vkCmdResetEvent");
	device->vkCmdResetQueryPool = (PFN_vkCmdResetQueryPool)
        get_vk_device_proc_address(device, "vkCmdResetQueryPool");
	device->vkCmdResolveImage = (PFN_vkCmdResolveImage)
        get_vk_device_proc_address(device, "vkCmdResolveImage");
	device->vkCmdSetBlendConstants = (PFN_vkCmdSetBlendConstants)
        get_vk_device_proc_address(device, "vkCmdSetBlendConstants");
	device->vkCmdSetDepthBias = (PFN_vkCmdSetDepthBias)
        get_vk_device_proc_address(device, "vkCmdSetDepthBias");
	device->vkCmdSetDepthBounds = (PFN_vkCmdSetDepthBounds)
        get_vk_device_proc_address(device, "vkCmdSetDepthBounds");
	device->vkCmdSetEvent = (PFN_vkCmdSetEvent)
        get_vk_device_proc_address(device, "vkCmdSetEvent");
	device->vkCmdSetLineWidth = (PFN_vkCmdSetLineWidth)
        get_vk_device_proc_address(device, "vkCmdSetLineWidth");
	device->vkCmdSetScissor = (PFN_vkCmdSetScissor)
        get_vk_device_proc_address(device, "vkCmdSetScissor");
	device->vkCmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask)
        get_vk_device_proc_address(device, "vkCmdSetStencilCompareMask");
	device->vkCmdSetStencilReference = (PFN_vkCmdSetStencilReference)
        get_vk_device_proc_address(device, "vkCmdSetStencilReference");
	device->vkCmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask)
        get_vk_device_proc_address(device, "vkCmdSetStencilWriteMask");
	device->vkCmdSetViewport = (PFN_vkCmdSetViewport)
        get_vk_device_proc_address(device, "vkCmdSetViewport");
	device->vkCmdUpdateBuffer = (PFN_vkCmdUpdateBuffer)
        get_vk_device_proc_address(device, "vkCmdUpdateBuffer");
	device->vkCmdWaitEvents = (PFN_vkCmdWaitEvents)
        get_vk_device_proc_address(device, "vkCmdWaitEvents");
	device->vkCmdWriteTimestamp = (PFN_vkCmdWriteTimestamp)
        get_vk_device_proc_address(device, "vkCmdWriteTimestamp");
	device->vkCreateBuffer = (PFN_vkCreateBuffer)
        get_vk_device_proc_address(device, "vkCreateBuffer");
	device->vkCreateBufferView = (PFN_vkCreateBufferView)
        get_vk_device_proc_address(device, "vkCreateBufferView");
	device->vkCreateCommandPool = (PFN_vkCreateCommandPool)
        get_vk_device_proc_address(device, "vkCreateCommandPool");
	device->vkCreateComputePipelines = (PFN_vkCreateComputePipelines)
        get_vk_device_proc_address(device, "vkCreateComputePipelines");
	device->vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)
        get_vk_device_proc_address(device, "vkCreateDescriptorPool");
	device->vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)
        get_vk_device_proc_address(device, "vkCreateDescriptorSetLayout");
	device->vkCreateEvent = (PFN_vkCreateEvent)
        get_vk_device_proc_address(device, "vkCreateEvent");
	device->vkCreateFence = (PFN_vkCreateFence)
        get_vk_device_proc_address(device, "vkCreateFence");
	device->vkCreateFramebuffer = (PFN_vkCreateFramebuffer)
        get_vk_device_proc_address(device, "vkCreateFramebuffer");
	device->vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)
        get_vk_device_proc_address(device, "vkCreateGraphicsPipelines");
	device->vkCreateImage = (PFN_vkCreateImage)
        get_vk_device_proc_address(device, "vkCreateImage");
	device->vkCreateImageView = (PFN_vkCreateImageView)
        get_vk_device_proc_address(device, "vkCreateImageView");
	device->vkCreatePipelineCache = (PFN_vkCreatePipelineCache)
        get_vk_device_proc_address(device, "vkCreatePipelineCache");
	device->vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)
        get_vk_device_proc_address(device, "vkCreatePipelineLayout");
	device->vkCreateQueryPool = (PFN_vkCreateQueryPool)
        get_vk_device_proc_address(device, "vkCreateQueryPool");
	device->vkCreateRenderPass = (PFN_vkCreateRenderPass)
        get_vk_device_proc_address(device, "vkCreateRenderPass");
	device->vkCreateSampler = (PFN_vkCreateSampler)
        get_vk_device_proc_address(device, "vkCreateSampler");
	device->vkCreateSemaphore = (PFN_vkCreateSemaphore)
        get_vk_device_proc_address(device, "vkCreateSemaphore");
	device->vkCreateShaderModule = (PFN_vkCreateShaderModule)
        get_vk_device_proc_address(device, "vkCreateShaderModule");
	device->vkDestroyBuffer = (PFN_vkDestroyBuffer)
        get_vk_device_proc_address(device, "vkDestroyBuffer");
	device->vkDestroyBufferView = (PFN_vkDestroyBufferView)
        get_vk_device_proc_address(device, "vkDestroyBufferView");
	device->vkDestroyCommandPool = (PFN_vkDestroyCommandPool)
        get_vk_device_proc_address(device, "vkDestroyCommandPool");
	device->vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)
        get_vk_device_proc_address(device, "vkDestroyDescriptorPool");
	device->vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)
        get_vk_device_proc_address(device, "vkDestroyDescriptorSetLayout");
	device->vkDestroyDevice = (PFN_vkDestroyDevice)
        get_vk_device_proc_address(device, "vkDestroyDevice");
	device->vkDestroyEvent = (PFN_vkDestroyEvent)
        get_vk_device_proc_address(device, "vkDestroyEvent");
	device->vkDestroyFence = (PFN_vkDestroyFence)
        get_vk_device_proc_address(device, "vkDestroyFence");
	device->vkDestroyFramebuffer = (PFN_vkDestroyFramebuffer)
        get_vk_device_proc_address(device, "vkDestroyFramebuffer");
	device->vkDestroyImage = (PFN_vkDestroyImage)
        get_vk_device_proc_address(device, "vkDestroyImage");
	device->vkDestroyImageView = (PFN_vkDestroyImageView)
        get_vk_device_proc_address(device, "vkDestroyImageView");
	device->vkDestroyPipeline = (PFN_vkDestroyPipeline)
        get_vk_device_proc_address(device, "vkDestroyPipeline");
	device->vkDestroyPipelineCache = (PFN_vkDestroyPipelineCache)
        get_vk_device_proc_address(device, "vkDestroyPipelineCache");
	device->vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)
        get_vk_device_proc_address(device, "vkDestroyPipelineLayout");
	device->vkDestroyQueryPool = (PFN_vkDestroyQueryPool)
        get_vk_device_proc_address(device, "vkDestroyQueryPool");
	device->vkDestroyRenderPass = (PFN_vkDestroyRenderPass)
        get_vk_device_proc_address(device, "vkDestroyRenderPass");
	device->vkDestroySampler = (PFN_vkDestroySampler)
        get_vk_device_proc_address(device, "vkDestroySampler");
	device->vkDestroySemaphore = (PFN_vkDestroySemaphore)
        get_vk_device_proc_address(device, "vkDestroySemaphore");
	device->vkDestroyShaderModule = (PFN_vkDestroyShaderModule)
        get_vk_device_proc_address(device, "vkDestroyShaderModule");
	device->vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)
        get_vk_device_proc_address(device, "vkDeviceWaitIdle");
	device->vkEndCommandBuffer = (PFN_vkEndCommandBuffer)
        get_vk_device_proc_address(device, "vkEndCommandBuffer");
	device->vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)
        get_vk_device_proc_address(device, "vkFlushMappedMemoryRanges");
	device->vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers)
        get_vk_device_proc_address(device, "vkFreeCommandBuffers");
	device->vkFreeDescriptorSets = (PFN_vkFreeDescriptorSets)
        get_vk_device_proc_address(device, "vkFreeDescriptorSets");
	device->vkFreeMemory = (PFN_vkFreeMemory)
        get_vk_device_proc_address(device, "vkFreeMemory");
	device->vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)
        get_vk_device_proc_address(device, "vkGetBufferMemoryRequirements");
	device->vkGetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment)
        get_vk_device_proc_address(device, "vkGetDeviceMemoryCommitment");
	device->vkGetDeviceQueue = (PFN_vkGetDeviceQueue)
        get_vk_device_proc_address(device, "vkGetDeviceQueue");
	device->vkGetEventStatus = (PFN_vkGetEventStatus)
        get_vk_device_proc_address(device, "vkGetEventStatus");
	device->vkGetFenceStatus = (PFN_vkGetFenceStatus)
        get_vk_device_proc_address(device, "vkGetFenceStatus");
	device->vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)
        get_vk_device_proc_address(device, "vkGetImageMemoryRequirements");
	device->vkGetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements)
        get_vk_device_proc_address(device, "vkGetImageSparseMemoryRequirements");
	device->vkGetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout)
        get_vk_device_proc_address(device, "vkGetImageSubresourceLayout");
	device->vkGetPipelineCacheData = (PFN_vkGetPipelineCacheData)
        get_vk_device_proc_address(device, "vkGetPipelineCacheData");
	device->vkGetQueryPoolResults = (PFN_vkGetQueryPoolResults)
        get_vk_device_proc_address(device, "vkGetQueryPoolResults");
	device->vkGetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity)
        get_vk_device_proc_address(device, "vkGetRenderAreaGranularity");
	device->vkInvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges)
        get_vk_device_proc_address(device, "vkInvalidateMappedMemoryRanges");
	device->vkMapMemory = (PFN_vkMapMemory)
        get_vk_device_proc_address(device, "vkMapMemory");
	device->vkMergePipelineCaches = (PFN_vkMergePipelineCaches)
        get_vk_device_proc_address(device, "vkMergePipelineCaches");
	device->vkQueueBindSparse = (PFN_vkQueueBindSparse)
        get_vk_device_proc_address(device, "vkQueueBindSparse");
	device->vkQueueSubmit = (PFN_vkQueueSubmit)
        get_vk_device_proc_address(device, "vkQueueSubmit");
	device->vkQueueWaitIdle = (PFN_vkQueueWaitIdle)
        get_vk_device_proc_address(device, "vkQueueWaitIdle");
	device->vkResetCommandBuffer = (PFN_vkResetCommandBuffer)
        get_vk_device_proc_address(device, "vkResetCommandBuffer");
	device->vkResetCommandPool = (PFN_vkResetCommandPool)
        get_vk_device_proc_address(device, "vkResetCommandPool");
	device->vkResetDescriptorPool = (PFN_vkResetDescriptorPool)
        get_vk_device_proc_address(device, "vkResetDescriptorPool");
	device->vkResetEvent = (PFN_vkResetEvent)
        get_vk_device_proc_address(device, "vkResetEvent");
	device->vkResetFences = (PFN_vkResetFences)
        get_vk_device_proc_address(device, "vkResetFences");
	device->vkSetEvent = (PFN_vkSetEvent)
        get_vk_device_proc_address(device, "vkSetEvent");
	device->vkUnmapMemory = (PFN_vkUnmapMemory)
        get_vk_device_proc_address(device, "vkUnmapMemory");
	device->vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)
        get_vk_device_proc_address(device, "vkUpdateDescriptorSets");
	device->vkWaitForFences = (PFN_vkWaitForFences)
        get_vk_device_proc_address(device, "vkWaitForFences");

	if (!device->vkAllocateCommandBuffers ||
	    !device->vkAllocateDescriptorSets ||
	    !device->vkAllocateMemory ||
	    !device->vkBeginCommandBuffer ||
	    !device->vkBindBufferMemory ||
	    !device->vkBindImageMemory ||
	    !device->vkCmdBeginQuery ||
	    !device->vkCmdBeginRenderPass ||
	    !device->vkCmdBindDescriptorSets ||
	    !device->vkCmdBindIndexBuffer ||
	    !device->vkCmdBindPipeline ||
	    !device->vkCmdBindVertexBuffers ||
	    !device->vkCmdBlitImage ||
	    !device->vkCmdClearAttachments ||
	    !device->vkCmdClearColorImage ||
	    !device->vkCmdClearDepthStencilImage ||
	    !device->vkCmdCopyBuffer ||
	    !device->vkCmdCopyBufferToImage ||
	    !device->vkCmdCopyImage ||
	    !device->vkCmdCopyImageToBuffer ||
	    !device->vkCmdCopyQueryPoolResults ||
	    !device->vkCmdDispatch ||
	    !device->vkCmdDispatchIndirect ||
	    !device->vkCmdDraw ||
	    !device->vkCmdDrawIndexed ||
	    !device->vkCmdDrawIndexedIndirect ||
	    !device->vkCmdDrawIndirect ||
	    !device->vkCmdEndQuery ||
	    !device->vkCmdEndRenderPass ||
	    !device->vkCmdExecuteCommands ||
	    !device->vkCmdFillBuffer ||
	    !device->vkCmdNextSubpass ||
	    !device->vkCmdPipelineBarrier ||
	    !device->vkCmdPushConstants ||
	    !device->vkCmdResetEvent ||
	    !device->vkCmdResetQueryPool ||
	    !device->vkCmdResolveImage ||
	    !device->vkCmdSetBlendConstants ||
	    !device->vkCmdSetDepthBias ||
	    !device->vkCmdSetDepthBounds ||
	    !device->vkCmdSetEvent ||
	    !device->vkCmdSetLineWidth ||
	    !device->vkCmdSetScissor ||
	    !device->vkCmdSetStencilCompareMask ||
	    !device->vkCmdSetStencilReference ||
	    !device->vkCmdSetStencilWriteMask ||
	    !device->vkCmdSetViewport ||
	    !device->vkCmdUpdateBuffer ||
	    !device->vkCmdWaitEvents ||
	    !device->vkCmdWriteTimestamp ||
	    !device->vkCreateBuffer ||
	    !device->vkCreateBufferView ||
	    !device->vkCreateCommandPool ||
	    !device->vkCreateComputePipelines ||
	    !device->vkCreateDescriptorPool ||
	    !device->vkCreateDescriptorSetLayout ||
	    !device->vkCreateEvent ||
	    !device->vkCreateFence ||
	    !device->vkCreateFramebuffer ||
	    !device->vkCreateGraphicsPipelines ||
	    !device->vkCreateImage ||
	    !device->vkCreateImageView ||
	    !device->vkCreatePipelineCache ||
	    !device->vkCreatePipelineLayout ||
	    !device->vkCreateQueryPool ||
	    !device->vkCreateRenderPass ||
	    !device->vkCreateSampler ||
	    !device->vkCreateSemaphore ||
	    !device->vkCreateShaderModule ||
	    !device->vkDestroyBuffer ||
	    !device->vkDestroyBufferView ||
	    !device->vkDestroyCommandPool ||
	    !device->vkDestroyDescriptorPool ||
	    !device->vkDestroyDescriptorSetLayout ||
	    !device->vkDestroyDevice ||
	    !device->vkDestroyEvent ||
	    !device->vkDestroyFence ||
	    !device->vkDestroyFramebuffer ||
	    !device->vkDestroyImage ||
	    !device->vkDestroyImageView ||
	    !device->vkDestroyPipeline ||
	    !device->vkDestroyPipelineCache ||
	    !device->vkDestroyPipelineLayout ||
	    !device->vkDestroyQueryPool ||
	    !device->vkDestroyRenderPass ||
	    !device->vkDestroySampler ||
	    !device->vkDestroySemaphore ||
	    !device->vkDestroyShaderModule ||
	    !device->vkDeviceWaitIdle ||
	    !device->vkEndCommandBuffer ||
	    !device->vkFlushMappedMemoryRanges ||
	    !device->vkFreeCommandBuffers ||
	    !device->vkFreeDescriptorSets ||
	    !device->vkFreeMemory ||
	    !device->vkGetBufferMemoryRequirements ||
	    !device->vkGetDeviceMemoryCommitment ||
	    !device->vkGetDeviceQueue ||
	    !device->vkGetEventStatus ||
	    !device->vkGetFenceStatus ||
	    !device->vkGetImageMemoryRequirements ||
	    !device->vkGetImageSparseMemoryRequirements ||
	    !device->vkGetImageSubresourceLayout ||
	    !device->vkGetPipelineCacheData ||
	    !device->vkGetQueryPoolResults ||
	    !device->vkGetRenderAreaGranularity ||
	    !device->vkInvalidateMappedMemoryRanges ||
	    !device->vkMapMemory ||
	    !device->vkMergePipelineCaches ||
	    !device->vkQueueBindSparse ||
	    !device->vkQueueSubmit ||
	    !device->vkQueueWaitIdle ||
	    !device->vkResetCommandBuffer ||
	    !device->vkResetCommandPool ||
	    !device->vkResetDescriptorPool ||
	    !device->vkResetEvent ||
	    !device->vkResetFences ||
	    !device->vkSetEvent ||
	    !device->vkUnmapMemory ||
	    !device->vkUpdateDescriptorSets ||
	    !device->vkWaitForFences)
        return false;

    /* core 1.1 */
    device->vkBindBufferMemory2 = (PFN_vkBindBufferMemory2)
        get_vk_device_proc_address(device, "vkBindBufferMemory2");
    device->vkBindImageMemory2 = (PFN_vkBindImageMemory2)
        get_vk_device_proc_address(device, "vkBindImageMemory2");
    device->vkCmdDispatchBase = (PFN_vkCmdDispatchBase)
        get_vk_device_proc_address(device, "vkCmdDispatchBase");
    device->vkCmdSetDeviceMask = (PFN_vkCmdSetDeviceMask)
        get_vk_device_proc_address(device, "vkCmdSetDeviceMask");
    device->vkCreateDescriptorUpdateTemplate = (PFN_vkCreateDescriptorUpdateTemplate)
        get_vk_device_proc_address(device, "vkCreateDescriptorUpdateTemplate");
    device->vkCreateSamplerYcbcrConversion = (PFN_vkCreateSamplerYcbcrConversion)
        get_vk_device_proc_address(device, "vkCreateSamplerYcbcrConversion");
    device->vkDestroyDescriptorUpdateTemplate = (PFN_vkDestroyDescriptorUpdateTemplate)
        get_vk_device_proc_address(device, "vkDestroyDescriptorUpdateTemplate");
    device->vkDestroySamplerYcbcrConversion = (PFN_vkDestroySamplerYcbcrConversion)
        get_vk_device_proc_address(device, "vkDestroySamplerYcbcrConversion");
    device->vkGetBufferMemoryRequirements2 = (PFN_vkGetBufferMemoryRequirements2)
        get_vk_device_proc_address(device, "vkGetBufferMemoryRequirements2");
    device->vkGetDescriptorSetLayoutSupport = (PFN_vkGetDescriptorSetLayoutSupport)
        get_vk_device_proc_address(device, "vkGetDescriptorSetLayoutSupport");
    device->vkGetDeviceGroupPeerMemoryFeatures = (PFN_vkGetDeviceGroupPeerMemoryFeatures)
        get_vk_device_proc_address(device, "vkGetDeviceGroupPeerMemoryFeatures");
    device->vkGetDeviceQueue2 = (PFN_vkGetDeviceQueue2)
        get_vk_device_proc_address(device, "vkGetDeviceQueue2");
    device->vkGetImageMemoryRequirements2 = (PFN_vkGetImageMemoryRequirements2)
        get_vk_device_proc_address(device, "vkGetImageMemoryRequirements2");
    device->vkGetImageSparseMemoryRequirements2 = (PFN_vkGetImageSparseMemoryRequirements2)
        get_vk_device_proc_address(device, "vkGetImageSparseMemoryRequirements2");
    device->vkTrimCommandPool = (PFN_vkTrimCommandPool)
        get_vk_device_proc_address(device, "vkTrimCommandPool");
    device->vkUpdateDescriptorSetWithTemplate = (PFN_vkUpdateDescriptorSetWithTemplate)
        get_vk_device_proc_address(device, "vkUpdateDescriptorSetWithTemplate");

    if (!device->vkBindBufferMemory2 ||
        !device->vkBindImageMemory2 ||
        !device->vkCmdDispatchBase ||
        !device->vkCmdSetDeviceMask ||
        !device->vkCreateDescriptorUpdateTemplate ||
        !device->vkCreateSamplerYcbcrConversion ||
        !device->vkDestroyDescriptorUpdateTemplate ||
        !device->vkDestroySamplerYcbcrConversion ||
        !device->vkGetBufferMemoryRequirements2 ||
        !device->vkGetDescriptorSetLayoutSupport ||
        !device->vkGetDeviceGroupPeerMemoryFeatures ||
        !device->vkGetDeviceQueue2 ||
        !device->vkGetImageMemoryRequirements2 ||
        !device->vkGetImageSparseMemoryRequirements2 ||
        !device->vkTrimCommandPool ||
        !device->vkUpdateDescriptorSetWithTemplate)
        return false;

    /* core 1.2 */
    device->vkCmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount)
        get_vk_device_proc_address(device, "vkCmdDrawIndexedIndirectCount");
    device->vkCmdDrawIndirectCount = (PFN_vkCmdDrawIndirectCount)
        get_vk_device_proc_address(device, "vkCmdDrawIndirectCount");
    device->vkGetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress)
        get_vk_device_proc_address(device, "vkGetBufferDeviceAddress");
    device->vkGetBufferOpaqueCaptureAddress = (PFN_vkGetBufferOpaqueCaptureAddress)
        get_vk_device_proc_address(device, "vkGetBufferOpaqueCaptureAddress");
    device->vkGetDeviceMemoryOpaqueCaptureAddress = (PFN_vkGetDeviceMemoryOpaqueCaptureAddress)
        get_vk_device_proc_address(device, "vkGetDeviceMemoryOpaqueCaptureAddress");
    device->vkGetSemaphoreCounterValue = (PFN_vkGetSemaphoreCounterValue)
        get_vk_device_proc_address(device, "vkGetSemaphoreCounterValue");
    device->vkResetQueryPool = (PFN_vkResetQueryPool)
        get_vk_device_proc_address(device, "vkResetQueryPool");
    device->vkSignalSemaphore = (PFN_vkSignalSemaphore)
        get_vk_device_proc_address(device, "vkSignalSemaphore");
    device->vkWaitSemaphores = (PFN_vkWaitSemaphores)
        get_vk_device_proc_address(device, "vkWaitSemaphores");

    if (!device->vkCmdDrawIndexedIndirectCount ||
        !device->vkCmdDrawIndirectCount ||
        !device->vkGetBufferDeviceAddress ||
        !device->vkGetBufferOpaqueCaptureAddress ||
        !device->vkGetDeviceMemoryOpaqueCaptureAddress ||
        !device->vkGetSemaphoreCounterValue ||
        !device->vkResetQueryPool ||
        !device->vkSignalSemaphore ||
        !device->vkWaitSemaphores)
        return false;

    /* core 1.3 */
    if (device->header.details->api_version >= VK_API_VERSION_1_3) {
        device->vkCmdBeginRendering = (PFN_vkCmdBeginRendering)
            get_vk_device_proc_address(device, "vkCmdBeginRendering");
        device->vkCmdEndRendering = (PFN_vkCmdEndRendering)
            get_vk_device_proc_address(device, "vkCmdEndRendering");
        device->vkCmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2)
            get_vk_device_proc_address(device, "vkCmdPipelineBarrier2");
        device->vkCmdResetEvent2 = (PFN_vkCmdResetEvent2)
            get_vk_device_proc_address(device, "vkCmdResetEvent2");
        device->vkCmdSetEvent2 = (PFN_vkCmdSetEvent2)
            get_vk_device_proc_address(device, "vkCmdSetEvent2");
        device->vkCmdWaitEvents2 = (PFN_vkCmdWaitEvents2)
            get_vk_device_proc_address(device, "vkCmdWaitEvents2");
        device->vkCmdWriteTimestamp2 = (PFN_vkCmdWriteTimestamp2)
            get_vk_device_proc_address(device, "vkCmdWriteTimestamp2");
        device->vkQueueSubmit2 = (PFN_vkQueueSubmit2)
            get_vk_device_proc_address(device, "vkQueueSubmit2");
        device->vkGetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirements)
            get_vk_device_proc_address(device, "vkGetDeviceBufferMemoryRequirements");
        device->vkGetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirements)
            get_vk_device_proc_address(device, "vkGetDeviceImageMemoryRequirements");
        device->vkGetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirements)
            get_vk_device_proc_address(device, "vkGetDeviceImageSparseMemoryRequirements");
    } else { /* backwards compatibility 1.3 */
        u64 required = device_extension_khr_dynamic_rendering | device_extension_khr_synchronization2 | device_extension_khr_maintenance4;
        if ((extension_bits & (required)) != required)
            return false;

        device->vkCmdBeginRendering = (PFN_vkCmdBeginRenderingKHR)
            get_vk_device_proc_address(device, "vkCmdBeginRenderingKHR");
        device->vkCmdEndRendering = (PFN_vkCmdEndRenderingKHR)
            get_vk_device_proc_address(device, "vkCmdEndRenderingKHR");
        device->vkCmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2KHR)
            get_vk_device_proc_address(device, "vkCmdPipelineBarrier2KHR");
        device->vkCmdResetEvent2 = (PFN_vkCmdResetEvent2KHR)
            get_vk_device_proc_address(device, "vkCmdResetEvent2KHR");
        device->vkCmdSetEvent2 = (PFN_vkCmdSetEvent2KHR)
            get_vk_device_proc_address(device, "vkCmdSetEvent2KHR");
        device->vkCmdWaitEvents2 = (PFN_vkCmdWaitEvents2KHR)
            get_vk_device_proc_address(device, "vkCmdWaitEvents2KHR");
        device->vkCmdWriteTimestamp2 = (PFN_vkCmdWriteTimestamp2KHR)
            get_vk_device_proc_address(device, "vkCmdWriteTimestamp2KHR");
        device->vkQueueSubmit2 = (PFN_vkQueueSubmit2KHR)
            get_vk_device_proc_address(device, "vkQueueSubmit2KHR");
        device->vkGetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirementsKHR)
            get_vk_device_proc_address(device, "vkGetDeviceBufferMemoryRequirementsKHR");
        device->vkGetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirementsKHR)
            get_vk_device_proc_address(device, "vkGetDeviceImageMemoryRequirementsKHR");
        device->vkGetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirementsKHR)
            get_vk_device_proc_address(device, "vkGetDeviceImageSparseMemoryRequirementsKHR");
    }
    if (!device->vkCmdBeginRendering ||
        !device->vkCmdEndRendering ||
        !device->vkCmdPipelineBarrier2 ||
        !device->vkCmdResetEvent2 ||
        !device->vkCmdSetEvent2 ||
        !device->vkCmdWaitEvents2 ||
        !device->vkCmdWriteTimestamp2 ||
        !device->vkQueueSubmit2 ||
        !device->vkGetDeviceBufferMemoryRequirements ||
        !device->vkGetDeviceImageMemoryRequirements ||
        !device->vkGetDeviceImageSparseMemoryRequirements)
        return false;

    /* core 1.4 */
    if (device->header.details->api_version >= VK_API_VERSION_1_4) {
        device->vkCmdBindDescriptorSets2 = (PFN_vkCmdBindDescriptorSets2)
            get_vk_device_proc_address(device, "vkCmdBindDescriptorSets2");
        device->vkCmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2)
            get_vk_device_proc_address(device, "vkCmdBindIndexBuffer2");
        device->vkCmdPushConstants2 = (PFN_vkCmdPushConstants2)
            get_vk_device_proc_address(device, "vkCmdPushConstants2");
        device->vkCmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocations)
            get_vk_device_proc_address(device, "vkCmdSetRenderingAttachmentLocations");
        device->vkCmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndices)
            get_vk_device_proc_address(device, "vkCmdSetRenderingInputAttachmentIndices");
        device->vkGetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayout)
            get_vk_device_proc_address(device, "vkGetDeviceImageSubresourceLayout");
        device->vkGetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2)
            get_vk_device_proc_address(device, "vkGetImageSubresourceLayout2");
        device->vkGetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularity)
            get_vk_device_proc_address(device, "vkGetRenderingAreaGranularity");

        if (!device->vkCmdBindDescriptorSets2 ||
            !device->vkCmdBindIndexBuffer2 ||
            !device->vkCmdPushConstants2 ||
            !device->vkCmdSetRenderingAttachmentLocations ||
            !device->vkCmdSetRenderingInputAttachmentIndices ||
            !device->vkGetDeviceImageSubresourceLayout ||
            !device->vkGetImageSubresourceLayout2 ||
            !device->vkGetRenderingAreaGranularity)
            return false;
    } else { /* backwards compatibility 1.4 */
        u64 required = device_extension_khr_dynamic_rendering_local_read | device_extension_khr_maintenance5;
        if ((extension_bits & (required)) != required)
            return false;

        if (extension_bits & device_extension_khr_dynamic_rendering_local_read) {
            device->vkCmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocationsKHR)
                get_vk_device_proc_address(device, "vkCmdSetRenderingAttachmentLocationsKHR");
            device->vkCmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndicesKHR)
                get_vk_device_proc_address(device, "vkCmdSetRenderingInputAttachmentIndicesKHR");

            if (!device->vkCmdSetRenderingAttachmentLocations || !device->vkCmdSetRenderingInputAttachmentIndices)
                return false;
        }

        if (extension_bits & device_extension_khr_maintenance6) {
            device->vkCmdBindDescriptorSets2 = (PFN_vkCmdBindDescriptorSets2KHR)
                get_vk_device_proc_address(device, "vkCmdBindDescriptorSets2KHR");
            device->vkCmdPushConstants2 = (PFN_vkCmdPushConstants2KHR)
                get_vk_device_proc_address(device, "vkCmdPushConstants2KHR");

            if (!device->vkCmdBindDescriptorSets2 || !device->vkCmdPushConstants2)
                return false;
        }

        if (extension_bits & device_extension_khr_maintenance5) {
            device->vkCmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2KHR)
                get_vk_device_proc_address(device, "vkCmdBindIndexBuffer2KHR");
            device->vkGetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayoutKHR)
                get_vk_device_proc_address(device, "vkGetDeviceImageSubresourceLayoutKHR");
            device->vkGetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2KHR)
                get_vk_device_proc_address(device, "vkGetImageSubresourceLayout2KHR");
            device->vkGetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularityKHR)
                get_vk_device_proc_address(device, "vkGetRenderingAreaGranularityKHR");

            if (!device->vkCmdBindIndexBuffer2 ||
                !device->vkGetDeviceImageSubresourceLayout ||
                !device->vkGetImageSubresourceLayout2 ||
                !device->vkGetRenderingAreaGranularity)
                return false;
        }
    }

    /* swapchain */
    if (extension_bits & device_extension_khr_swapchain) {
        device->vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)
            get_vk_device_proc_address(device, "vkAcquireNextImageKHR");
        device->vkAcquireNextImage2KHR = (PFN_vkAcquireNextImage2KHR)
            get_vk_device_proc_address(device, "vkAcquireNextImage2KHR");
        device->vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)
            get_vk_device_proc_address(device, "vkCreateSwapchainKHR");
        device->vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)
            get_vk_device_proc_address(device, "vkDestroySwapchainKHR");
        device->vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)
            get_vk_device_proc_address(device, "vkGetSwapchainImagesKHR");
        device->vkQueuePresentKHR = (PFN_vkQueuePresentKHR)
            get_vk_device_proc_address(device, "vkQueuePresentKHR");

        if (!device->vkAcquireNextImageKHR ||
            !device->vkAcquireNextImage2KHR ||
            !device->vkCreateSwapchainKHR ||
            !device->vkDestroySwapchainKHR ||
            !device->vkGetSwapchainImagesKHR ||
            !device->vkQueuePresentKHR)
            return false;
    }

    /* device fault */
    if (extension_bits & device_extension_ext_device_fault) {
        device->vkGetDeviceFaultInfoEXT = (PFN_vkGetDeviceFaultInfoEXT)
            get_vk_device_proc_address(device, "vkGetDeviceFaultInfoEXT");
        if (!device->vkGetDeviceFaultInfoEXT) return false;
    }

    /* dynamic state 3 */
    if (extension_bits & device_extension_ext_extended_dynamic_state3) {
        device->vkCmdSetRasterizationSamplesEXT = (PFN_vkCmdSetRasterizationSamplesEXT)
            get_vk_device_proc_address(device, "vkCmdSetRasterizationSamplesEXT");
        if (!device->vkCmdSetRasterizationSamplesEXT) return false;
    }

    /* mesh shader */
    if (extension_bits & device_extension_ext_mesh_shader) {
        device->vkCmdDrawMeshTasksEXT = (PFN_vkCmdDrawMeshTasksEXT)
            get_vk_device_proc_address(device, "vkCmdDrawMeshTasksEXT");
        device->vkCmdDrawMeshTasksIndirectEXT = (PFN_vkCmdDrawMeshTasksIndirectEXT)
            get_vk_device_proc_address(device, "vkCmdDrawMeshTasksIndirectEXT");
        device->vkCmdDrawMeshTasksIndirectCountEXT = (PFN_vkCmdDrawMeshTasksIndirectCountEXT)
            get_vk_device_proc_address(device, "vkCmdDrawMeshTasksIndirectCountEXT");

        if (!device->vkCmdDrawMeshTasksEXT ||
            !device->vkCmdDrawMeshTasksIndirectEXT ||
            !device->vkCmdDrawMeshTasksIndirectCountEXT)
            return false;
    }

    /* deferred host operations */
    if (extension_bits & device_extension_khr_deferred_host_operations) {
        device->vkCreateDeferredOperationKHR = (PFN_vkCreateDeferredOperationKHR)
            get_vk_device_proc_address(device, "vkCreateDeferredOperationKHR");
        device->vkDeferredOperationJoinKHR = (PFN_vkDeferredOperationJoinKHR)
            get_vk_device_proc_address(device, "vkDeferredOperationJoinKHR");
        device->vkDestroyDeferredOperationKHR = (PFN_vkDestroyDeferredOperationKHR)
            get_vk_device_proc_address(device, "vkDestroyDeferredOperationKHR");
        device->vkGetDeferredOperationMaxConcurrencyKHR = (PFN_vkGetDeferredOperationMaxConcurrencyKHR)
            get_vk_device_proc_address(device, "vkGetDeferredOperationMaxConcurrencyKHR");
        device->vkGetDeferredOperationResultKHR = (PFN_vkGetDeferredOperationResultKHR)
            get_vk_device_proc_address(device, "vkGetDeferredOperationResultKHR");

        if (!device->vkCreateDeferredOperationKHR ||
            !device->vkDeferredOperationJoinKHR ||
            !device->vkDestroyDeferredOperationKHR ||
            !device->vkGetDeferredOperationMaxConcurrencyKHR ||
            !device->vkGetDeferredOperationResultKHR)
            return false;
    }

    /* acceleration structure */
    if (extension_bits & device_extension_khr_acceleration_structure) {
        device->vkBuildAccelerationStructuresKHR = (PFN_vkBuildAccelerationStructuresKHR)
            get_vk_device_proc_address(device, "vkBuildAccelerationStructuresKHR");
        device->vkCmdBuildAccelerationStructuresIndirectKHR = (PFN_vkCmdBuildAccelerationStructuresIndirectKHR)
            get_vk_device_proc_address(device, "vkCmdBuildAccelerationStructuresIndirectKHR");
        device->vkCmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR)
            get_vk_device_proc_address(device, "vkCmdBuildAccelerationStructuresKHR");
        device->vkCmdCopyAccelerationStructureKHR = (PFN_vkCmdCopyAccelerationStructureKHR)
            get_vk_device_proc_address(device, "vkCmdCopyAccelerationStructureKHR");
        device->vkCmdCopyAccelerationStructureToMemoryKHR = (PFN_vkCmdCopyAccelerationStructureToMemoryKHR)
            get_vk_device_proc_address(device, "vkCmdCopyAccelerationStructureToMemoryKHR");
        device->vkCmdCopyMemoryToAccelerationStructureKHR = (PFN_vkCmdCopyMemoryToAccelerationStructureKHR)
            get_vk_device_proc_address(device, "vkCmdCopyMemoryToAccelerationStructureKHR");
        device->vkCmdWriteAccelerationStructuresPropertiesKHR = (PFN_vkCmdWriteAccelerationStructuresPropertiesKHR)
            get_vk_device_proc_address(device, "vkCmdWriteAccelerationStructuresPropertiesKHR");
        device->vkCopyAccelerationStructureKHR = (PFN_vkCopyAccelerationStructureKHR)
            get_vk_device_proc_address(device, "vkCopyAccelerationStructureKHR");
        device->vkCopyAccelerationStructureToMemoryKHR = (PFN_vkCopyAccelerationStructureToMemoryKHR)
            get_vk_device_proc_address(device, "vkCopyAccelerationStructureToMemoryKHR");
        device->vkCopyMemoryToAccelerationStructureKHR = (PFN_vkCopyMemoryToAccelerationStructureKHR)
            get_vk_device_proc_address(device, "vkCopyMemoryToAccelerationStructureKHR");
        device->vkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)
            get_vk_device_proc_address(device, "vkCreateAccelerationStructureKHR");
        device->vkDestroyAccelerationStructureKHR = (PFN_vkDestroyAccelerationStructureKHR)
            get_vk_device_proc_address(device, "vkDestroyAccelerationStructureKHR");
        device->vkGetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR)
            get_vk_device_proc_address(device, "vkGetAccelerationStructureBuildSizesKHR");
        device->vkGetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)
            get_vk_device_proc_address(device, "vkGetAccelerationStructureDeviceAddressKHR");
        device->vkGetDeviceAccelerationStructureCompatibilityKHR = (PFN_vkGetDeviceAccelerationStructureCompatibilityKHR)
            get_vk_device_proc_address(device, "vkGetDeviceAccelerationStructureCompatibilityKHR");
        device->vkWriteAccelerationStructuresPropertiesKHR = (PFN_vkWriteAccelerationStructuresPropertiesKHR)
            get_vk_device_proc_address(device, "vkWriteAccelerationStructuresPropertiesKHR");

        if (!device->vkBuildAccelerationStructuresKHR ||
            !device->vkCmdBuildAccelerationStructuresIndirectKHR ||
            !device->vkCmdBuildAccelerationStructuresKHR ||
            !device->vkCmdCopyAccelerationStructureKHR ||
            !device->vkCmdCopyAccelerationStructureToMemoryKHR ||
            !device->vkCmdCopyMemoryToAccelerationStructureKHR ||
            !device->vkCmdWriteAccelerationStructuresPropertiesKHR ||
            !device->vkCopyAccelerationStructureKHR ||
            !device->vkCopyAccelerationStructureToMemoryKHR ||
            !device->vkCopyMemoryToAccelerationStructureKHR ||
            !device->vkCreateAccelerationStructureKHR ||
            !device->vkDestroyAccelerationStructureKHR ||
            !device->vkGetAccelerationStructureBuildSizesKHR ||
            !device->vkGetAccelerationStructureDeviceAddressKHR ||
            !device->vkGetDeviceAccelerationStructureCompatibilityKHR ||
            !device->vkWriteAccelerationStructuresPropertiesKHR)
            return false;
    }

    /* raytracing pipeline */
    if (extension_bits & device_extension_khr_ray_tracing_pipeline) {
        device->vkCmdSetRayTracingPipelineStackSizeKHR = (PFN_vkCmdSetRayTracingPipelineStackSizeKHR)
            get_vk_device_proc_address(device, "vkCmdSetRayTracingPipelineStackSizeKHR");
        device->vkCmdTraceRaysIndirectKHR = (PFN_vkCmdTraceRaysIndirectKHR)
            get_vk_device_proc_address(device, "vkCmdTraceRaysIndirectKHR");
        device->vkCmdTraceRaysKHR = (PFN_vkCmdTraceRaysKHR)
            get_vk_device_proc_address(device, "vkCmdTraceRaysKHR");
        device->vkCreateRayTracingPipelinesKHR = (PFN_vkCreateRayTracingPipelinesKHR)
            get_vk_device_proc_address(device, "vkCreateRayTracingPipelinesKHR");
        device->vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = (PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR)
            get_vk_device_proc_address(device, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
        device->vkGetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR)
            get_vk_device_proc_address(device, "vkGetRayTracingShaderGroupHandlesKHR");
        device->vkGetRayTracingShaderGroupStackSizeKHR = (PFN_vkGetRayTracingShaderGroupStackSizeKHR)
            get_vk_device_proc_address(device, "vkGetRayTracingShaderGroupStackSizeKHR");

        if (!device->vkCmdSetRayTracingPipelineStackSizeKHR ||
            !device->vkCmdTraceRaysIndirectKHR ||
            !device->vkCmdTraceRaysKHR ||
            !device->vkCreateRayTracingPipelinesKHR ||
            !device->vkGetRayTracingCaptureReplayShaderGroupHandlesKHR ||
            !device->vkGetRayTracingShaderGroupHandlesKHR ||
            !device->vkGetRayTracingShaderGroupStackSizeKHR)
            return false;
    }

    /* raytracing maintenance */
    if (extension_bits & device_extension_khr_ray_tracing_maintenance1) {
        device->vkCmdTraceRaysIndirect2KHR = (PFN_vkCmdTraceRaysIndirect2KHR)
            get_vk_device_proc_address(device, "vkCmdTraceRaysIndirect2KHR");
        if (!device->vkCmdTraceRaysIndirect2KHR) return false;
    }

    /* video coding */
    if (extension_bits & (device_extension_khr_video_queue | device_extension_khr_video_decode_queue)) {
        device->vkBindVideoSessionMemoryKHR = (PFN_vkBindVideoSessionMemoryKHR)
            get_vk_device_proc_address(device, "vkBindVideoSessionMemoryKHR");
        device->vkCmdBeginVideoCodingKHR = (PFN_vkCmdBeginVideoCodingKHR)
            get_vk_device_proc_address(device, "vkCmdBeginVideoCodingKHR");
        device->vkCmdControlVideoCodingKHR = (PFN_vkCmdControlVideoCodingKHR)
            get_vk_device_proc_address(device, "vkCmdControlVideoCodingKHR");
        device->vkCmdDecodeVideoKHR = (PFN_vkCmdDecodeVideoKHR)
            get_vk_device_proc_address(device, "vkCmdDecodeVideoKHR");
        device->vkCmdEncodeVideoKHR = (PFN_vkCmdEncodeVideoKHR)
            get_vk_device_proc_address(device, "vkCmdEncodeVideoKHR");
        device->vkCmdEndVideoCodingKHR = (PFN_vkCmdEndVideoCodingKHR)
            get_vk_device_proc_address(device, "vkCmdEndVideoCodingKHR");
        device->vkCreateVideoSessionKHR = (PFN_vkCreateVideoSessionKHR)
            get_vk_device_proc_address(device, "vkCreateVideoSessionKHR");
        device->vkCreateVideoSessionParametersKHR = (PFN_vkCreateVideoSessionParametersKHR)
            get_vk_device_proc_address(device, "vkCreateVideoSessionParametersKHR");
        device->vkDestroyVideoSessionKHR = (PFN_vkDestroyVideoSessionKHR)
            get_vk_device_proc_address(device, "vkDestroyVideoSessionKHR");
        device->vkDestroyVideoSessionParametersKHR = (PFN_vkDestroyVideoSessionParametersKHR)
            get_vk_device_proc_address(device, "vkDestroyVideoSessionParametersKHR");
        device->vkGetEncodedVideoSessionParametersKHR = (PFN_vkGetEncodedVideoSessionParametersKHR)
            get_vk_device_proc_address(device, "vkGetEncodedVideoSessionParametersKHR");
        device->vkGetVideoSessionMemoryRequirementsKHR = (PFN_vkGetVideoSessionMemoryRequirementsKHR)
            get_vk_device_proc_address(device, "vkGetVideoSessionMemoryRequirementsKHR");
        device->vkUpdateVideoSessionParametersKHR = (PFN_vkUpdateVideoSessionParametersKHR)
            get_vk_device_proc_address(device, "vkUpdateVideoSessionParametersKHR");

        if (!device->vkBindVideoSessionMemoryKHR ||
            !device->vkCmdBeginVideoCodingKHR ||
            !device->vkCmdControlVideoCodingKHR ||
            !device->vkCmdDecodeVideoKHR ||
            !device->vkCmdEncodeVideoKHR ||
            !device->vkCmdEndVideoCodingKHR ||
            !device->vkCreateVideoSessionKHR ||
            !device->vkCreateVideoSessionParametersKHR ||
            !device->vkDestroyVideoSessionKHR ||
            !device->vkDestroyVideoSessionParametersKHR ||
            !device->vkGetEncodedVideoSessionParametersKHR ||
            !device->vkGetVideoSessionMemoryRequirementsKHR ||
            !device->vkUpdateVideoSessionParametersKHR)
            return false;
    }

    /* work graph */
    if (extension_bits & device_extension_amdx_shader_enqueue) {
        device->vkCmdDispatchGraphAMDX = (PFN_vkCmdDispatchGraphAMDX)
            get_vk_device_proc_address(device, "vkCmdDispatchGraphAMDX");
        device->vkCmdDispatchGraphIndirectAMDX = (PFN_vkCmdDispatchGraphIndirectAMDX)
            get_vk_device_proc_address(device, "vkCmdDispatchGraphIndirectAMDX");
        device->vkCmdDispatchGraphIndirectCountAMDX = (PFN_vkCmdDispatchGraphIndirectCountAMDX)
            get_vk_device_proc_address(device, "vkCmdDispatchGraphIndirectCountAMDX");
        device->vkCmdInitializeGraphScratchMemoryAMDX = (PFN_vkCmdInitializeGraphScratchMemoryAMDX)
            get_vk_device_proc_address(device, "vkCmdInitializeGraphScratchMemoryAMDX");
        device->vkCreateExecutionGraphPipelinesAMDX = (PFN_vkCreateExecutionGraphPipelinesAMDX)
            get_vk_device_proc_address(device, "vkCreateExecutionGraphPipelinesAMDX");
        device->vkGetExecutionGraphPipelineNodeIndexAMDX = (PFN_vkGetExecutionGraphPipelineNodeIndexAMDX)
            get_vk_device_proc_address(device, "vkGetExecutionGraphPipelineNodeIndexAMDX");
        device->vkGetExecutionGraphPipelineScratchSizeAMDX = (PFN_vkGetExecutionGraphPipelineScratchSizeAMDX)
            get_vk_device_proc_address(device, "vkGetExecutionGraphPipelineScratchSizeAMDX");
        
        if (!device->vkCmdDispatchGraphAMDX ||
            !device->vkCmdDispatchGraphIndirectAMDX ||
            !device->vkCmdDispatchGraphIndirectCountAMDX ||
            !device->vkCmdInitializeGraphScratchMemoryAMDX ||
            !device->vkCreateExecutionGraphPipelinesAMDX ||
            !device->vkGetExecutionGraphPipelineNodeIndexAMDX ||
            !device->vkGetExecutionGraphPipelineScratchSizeAMDX)
            return false;
    }
    return true;
}

struct device_feature {
    u32 const  *vk_feature_offsets;
    u32         offsets_count;
    u32         feature_bit;
};
static constexpr u32 g_required_feature__descriptor_indexing_features[] = {
    lake_offsetof(struct physical_device_features, descriptor_indexing.shaderSampledImageArrayNonUniformIndexing),
    lake_offsetof(struct physical_device_features, descriptor_indexing.shaderStorageBufferArrayNonUniformIndexing),
    lake_offsetof(struct physical_device_features, descriptor_indexing.shaderStorageImageArrayNonUniformIndexing),
    lake_offsetof(struct physical_device_features, descriptor_indexing.descriptorBindingSampledImageUpdateAfterBind),
    lake_offsetof(struct physical_device_features, descriptor_indexing.descriptorBindingStorageImageUpdateAfterBind),
    lake_offsetof(struct physical_device_features, descriptor_indexing.descriptorBindingStorageBufferUpdateAfterBind),
    lake_offsetof(struct physical_device_features, descriptor_indexing.descriptorBindingUpdateUnusedWhilePending),
    lake_offsetof(struct physical_device_features, descriptor_indexing.descriptorBindingPartiallyBound),
    lake_offsetof(struct physical_device_features, descriptor_indexing.runtimeDescriptorArray),
};
static constexpr u32 g_required_feature__buffer_device_address_features[] = {
    lake_offsetof(struct physical_device_features, buffer_device_address.bufferDeviceAddress),
    lake_offsetof(struct physical_device_features, buffer_device_address.bufferDeviceAddressCaptureReplay),
    lake_offsetof(struct physical_device_features, buffer_device_address.bufferDeviceAddressMultiDevice),
};
static constexpr u32 g_required_feature__multi_draw_indirect_features[] = {
    lake_offsetof(struct physical_device_features, features2.features.multiDrawIndirect),
};
static constexpr u32 g_required_feature__tessellation_shader_features[] = {
    lake_offsetof(struct physical_device_features, features2.features.tessellationShader),
};
static constexpr u32 g_required_feature__depth_clamp_features[] = {
    lake_offsetof(struct physical_device_features, features2.features.depthClamp),
};
static constexpr u32 g_required_feature__sampler_anisotrophy_features[] = {
    lake_offsetof(struct physical_device_features, features2.features.samplerAnisotropy),
};
static constexpr u32 g_required_feature__framebuffer_local_dependencies_features[] = {
    lake_offsetof(struct physical_device_features, dynamic_rendering.dynamicRendering),
    lake_offsetof(struct physical_device_features, dynamic_rendering_local_read.dynamicRenderingLocalRead),
};
static constexpr u32 g_required_feature__timeline_semaphore_features[] = {
    lake_offsetof(struct physical_device_features, synchronization2.synchronization2),
    lake_offsetof(struct physical_device_features, timeline_semaphore.timelineSemaphore),
};
static constexpr u32 g_required_feature__fragment_stores_and_atomics_features[] = {
    lake_offsetof(struct physical_device_features, features2.features.fragmentStoresAndAtomics),
};
static constexpr u32 g_required_feature__texture_cube_array_features[] = {
    lake_offsetof(struct physical_device_features, features2.features.imageCubeArray),
};
static constexpr u32 g_required_feature__shader_storage_texture_features[] = {
    lake_offsetof(struct physical_device_features, features2.features.shaderStorageImageMultisample),
    lake_offsetof(struct physical_device_features, features2.features.shaderStorageImageReadWithoutFormat),
    lake_offsetof(struct physical_device_features, features2.features.shaderStorageImageWriteWithoutFormat),
};
static constexpr u32 g_required_feature__shader_int64_features[] = {
    lake_offsetof(struct physical_device_features, features2.features.shaderInt64),
};
static constexpr u32 g_required_feature__fill_mode_wireframe_features[] = {
    lake_offsetof(struct physical_device_features, features2.features.fillModeNonSolid),
    lake_offsetof(struct physical_device_features, features2.features.wideLines),
};
static constexpr u32 g_required_feature__resolve_host_query_data_features[] = {
    lake_offsetof(struct physical_device_features, host_query_reset.hostQueryReset),
};
static constexpr u32 g_required_feature__subgroup_size_control_features[] = {
    lake_offsetof(struct physical_device_features, subgroup_size_control.subgroupSizeControl),
    lake_offsetof(struct physical_device_features, subgroup_size_control.computeFullSubgroups),
};
static constexpr u32 g_required_feature__scalar_block_layout_features[] = {
    lake_offsetof(struct physical_device_features, scalar_block_layout.scalarBlockLayout),
};
static constexpr u32 g_required_feature__independent_blend_features[] = {
    lake_offsetof(struct physical_device_features, features2.features.independentBlend),
};
static constexpr u32 g_required_feature__variable_pointers_features[] = {
    lake_offsetof(struct physical_device_features, variable_pointer.variablePointers),
    lake_offsetof(struct physical_device_features, variable_pointer.variablePointersStorageBuffer),
};
static constexpr struct device_feature g_required_features[] = {
    (struct device_feature){ g_required_feature__descriptor_indexing_features, lake_arraysize(g_required_feature__descriptor_indexing_features), moon_missing_required_feature_descriptor_indexing},
    (struct device_feature){ g_required_feature__buffer_device_address_features, lake_arraysize(g_required_feature__buffer_device_address_features), moon_missing_required_feature_buffer_device_address},
    (struct device_feature){ g_required_feature__multi_draw_indirect_features, lake_arraysize(g_required_feature__multi_draw_indirect_features), moon_missing_required_feature_multi_draw_indirect},
    (struct device_feature){ g_required_feature__tessellation_shader_features, lake_arraysize(g_required_feature__tessellation_shader_features), moon_missing_required_feature_tessellation_shader},
    (struct device_feature){ g_required_feature__depth_clamp_features, lake_arraysize(g_required_feature__depth_clamp_features), moon_missing_required_feature_depth_clamp},
    (struct device_feature){ g_required_feature__sampler_anisotrophy_features, lake_arraysize(g_required_feature__sampler_anisotrophy_features), moon_missing_required_feature_sampler_anisotropy},
    (struct device_feature){ g_required_feature__framebuffer_local_dependencies_features, lake_arraysize(g_required_feature__framebuffer_local_dependencies_features), moon_missing_required_feature_framebuffer_local_dependencies},
    (struct device_feature){ g_required_feature__fragment_stores_and_atomics_features, lake_arraysize(g_required_feature__fragment_stores_and_atomics_features), moon_missing_required_feature_fragment_stores_and_atomics},
    (struct device_feature){ g_required_feature__timeline_semaphore_features, lake_arraysize(g_required_feature__timeline_semaphore_features), moon_missing_required_feature_timeline_semaphore},
    (struct device_feature){ g_required_feature__texture_cube_array_features, lake_arraysize(g_required_feature__texture_cube_array_features), moon_missing_required_feature_texture_cube_array},
    (struct device_feature){ g_required_feature__shader_storage_texture_features, lake_arraysize(g_required_feature__shader_storage_texture_features), moon_missing_required_feature_shader_storage_texture},
    (struct device_feature){ g_required_feature__shader_int64_features, lake_arraysize(g_required_feature__shader_int64_features), moon_missing_required_feature_shader_int64},
    (struct device_feature){ g_required_feature__fill_mode_wireframe_features, lake_arraysize(g_required_feature__fill_mode_wireframe_features), moon_missing_required_feature_fill_mode_wireframe},
    (struct device_feature){ g_required_feature__resolve_host_query_data_features, lake_arraysize(g_required_feature__resolve_host_query_data_features), moon_missing_required_feature_resolve_host_query_data},
    (struct device_feature){ g_required_feature__subgroup_size_control_features, lake_arraysize(g_required_feature__subgroup_size_control_features), moon_missing_required_feature_subgroup_size_control},
    (struct device_feature){ g_required_feature__scalar_block_layout_features, lake_arraysize(g_required_feature__scalar_block_layout_features), moon_missing_required_feature_scalar_block_layout },
    (struct device_feature){ g_required_feature__independent_blend_features, lake_arraysize(g_required_feature__independent_blend_features), moon_missing_required_feature_independent_blend },
    (struct device_feature){ g_required_feature__variable_pointers_features, lake_arraysize(g_required_feature__variable_pointers_features), moon_missing_required_feature_variable_pointers },
};
static constexpr u32 g_implicit_feature__mesh_shader_features[] = {
    lake_offsetof(struct physical_device_features, mesh_shader.meshShader),
    lake_offsetof(struct physical_device_features, mesh_shader.taskShader),
};
static constexpr u32 g_implicit_feature__basic_ray_tracing_features[] = {
    lake_offsetof(struct physical_device_features, acceleration_structure.accelerationStructure),
    lake_offsetof(struct physical_device_features, acceleration_structure.descriptorBindingAccelerationStructureUpdateAfterBind),
    lake_offsetof(struct physical_device_features, ray_query.rayQuery),
};
static constexpr u32 g_implicit_feature__ray_tracing_pipeline_features[] = {
    lake_offsetof(struct physical_device_features, acceleration_structure.accelerationStructure),
    lake_offsetof(struct physical_device_features, acceleration_structure.descriptorBindingAccelerationStructureUpdateAfterBind),
    lake_offsetof(struct physical_device_features, ray_query.rayQuery),
    lake_offsetof(struct physical_device_features, ray_tracing_pipeline.rayTracingPipeline),
    lake_offsetof(struct physical_device_features, ray_tracing_pipeline.rayTracingPipelineTraceRaysIndirect),
    lake_offsetof(struct physical_device_features, ray_tracing_pipeline.rayTraversalPrimitiveCulling),
};
static constexpr u32 g_implicit_feature__ray_tracing_invocation_reorder_features[] = {
    lake_offsetof(struct physical_device_features, acceleration_structure.accelerationStructure),
    lake_offsetof(struct physical_device_features, acceleration_structure.descriptorBindingAccelerationStructureUpdateAfterBind),
    lake_offsetof(struct physical_device_features, ray_query.rayQuery),
    lake_offsetof(struct physical_device_features, ray_tracing_invocation_reorder.rayTracingInvocationReorder),
};
static constexpr u32 g_implicit_feature__ray_tracing_position_fetch_features[] = {
    lake_offsetof(struct physical_device_features, acceleration_structure.accelerationStructure),
    lake_offsetof(struct physical_device_features, acceleration_structure.descriptorBindingAccelerationStructureUpdateAfterBind),
    lake_offsetof(struct physical_device_features, ray_query.rayQuery),
    lake_offsetof(struct physical_device_features, ray_tracing_position_fetch.rayTracingPositionFetch),
};
static constexpr u32 g_implicit_feature__conservative_rasterization_features[] = {
    lake_offsetof(struct physical_device_features, has_conservative_rasterization),
};
static constexpr u32 g_implicit_feature__work_graph_features[] = {
    lake_offsetof(struct physical_device_features, mesh_shader.meshShader),
    lake_offsetof(struct physical_device_features, mesh_shader.taskShader),
    lake_offsetof(struct physical_device_features, maintenance5.maintenance5),
    lake_offsetof(struct physical_device_features, shader_enqueue.shaderEnqueue),
    lake_offsetof(struct physical_device_features, shader_enqueue.shaderMeshEnqueue),
};
static constexpr u32 g_implicit_feature__image_atomic64_features[] = {
    lake_offsetof(struct physical_device_features, shader_image_atomic_int64.shaderImageInt64Atomics),
};
static constexpr u32 g_implicit_feature__shader_atomic_float_features[] = {
    lake_offsetof(struct physical_device_features, shader_atomic_float.shaderBufferFloat32Atomics),
    lake_offsetof(struct physical_device_features, shader_atomic_float.shaderBufferFloat32AtomicAdd),
    lake_offsetof(struct physical_device_features, shader_atomic_float.shaderSharedFloat32Atomics),
    lake_offsetof(struct physical_device_features, shader_atomic_float.shaderSharedFloat32AtomicAdd),
    lake_offsetof(struct physical_device_features, shader_atomic_float.shaderImageFloat32Atomics),
    lake_offsetof(struct physical_device_features, shader_atomic_float.shaderImageFloat32AtomicAdd),
};
static constexpr u32 g_implicit_feature__shader_atomic_int64_features[] = {
    lake_offsetof(struct physical_device_features, shader_atomic_int64.shaderBufferInt64Atomics),
    lake_offsetof(struct physical_device_features, shader_atomic_int64.shaderSharedInt64Atomics),
};
static constexpr u32 g_implicit_feature__shader_float16_features[] = {
    lake_offsetof(struct physical_device_features, shader_float16_int8.shaderFloat16),
};
static constexpr u32 g_implicit_feature__shader_int16_features[] = {
    lake_offsetof(struct physical_device_features, features2.features.shaderInt16),
    lake_offsetof(struct physical_device_features, bit16_storage.storageBuffer16BitAccess),
    lake_offsetof(struct physical_device_features, bit16_storage.uniformAndStorageBuffer16BitAccess),
    lake_offsetof(struct physical_device_features, bit16_storage.storagePushConstant16),
};
static constexpr u32 g_implicit_feature__shader_int8_features[] = {
    lake_offsetof(struct physical_device_features, shader_float16_int8.shaderInt8),
    lake_offsetof(struct physical_device_features, bit8_storage.storageBuffer8BitAccess),
    lake_offsetof(struct physical_device_features, bit8_storage.uniformAndStorageBuffer8BitAccess),
    lake_offsetof(struct physical_device_features, bit8_storage.storagePushConstant8),
};
static constexpr u32 g_implicit_feature__dynamic_state_features[] = {
    lake_offsetof(struct physical_device_features, extended_dynamic_state3.extendedDynamicState3RasterizationSamples),
};
static constexpr u32 g_implicit_feature__sparse_binding_features[] = {
    lake_offsetof(struct physical_device_features, features2.features.sparseBinding),
    lake_offsetof(struct physical_device_features, features2.features.sparseResidencyBuffer),
    lake_offsetof(struct physical_device_features, features2.features.sparseResidencyImage2D),
    lake_offsetof(struct physical_device_features, features2.features.sparseResidencyAliased),
};
static constexpr u32 g_implicit_feature__swapchain_features[] = {
    lake_offsetof(struct physical_device_features, has_swapchain),
};
static constexpr struct device_feature g_implicit_features[] = {
    (struct device_feature){ g_implicit_feature__mesh_shader_features, lake_arraysize(g_implicit_feature__mesh_shader_features), moon_implicit_feature_mesh_shader },
    (struct device_feature){ g_implicit_feature__basic_ray_tracing_features, lake_arraysize(g_implicit_feature__basic_ray_tracing_features), moon_implicit_feature_basic_ray_tracing },
    (struct device_feature){ g_implicit_feature__ray_tracing_pipeline_features, lake_arraysize(g_implicit_feature__ray_tracing_pipeline_features), moon_implicit_feature_ray_tracing_pipeline },
    (struct device_feature){ g_implicit_feature__ray_tracing_invocation_reorder_features, lake_arraysize(g_implicit_feature__ray_tracing_invocation_reorder_features), moon_implicit_feature_ray_tracing_invocation_reorder },
    (struct device_feature){ g_implicit_feature__ray_tracing_position_fetch_features, lake_arraysize(g_implicit_feature__ray_tracing_position_fetch_features), moon_implicit_feature_ray_tracing_position_fetch },
    (struct device_feature){ g_implicit_feature__conservative_rasterization_features, lake_arraysize(g_implicit_feature__conservative_rasterization_features), moon_implicit_feature_conservative_rasterization},
    (struct device_feature){ g_implicit_feature__work_graph_features, lake_arraysize(g_implicit_feature__work_graph_features), moon_implicit_feature_work_graph },
    (struct device_feature){ g_implicit_feature__image_atomic64_features, lake_arraysize(g_implicit_feature__image_atomic64_features), moon_implicit_feature_image_atomic64},
    (struct device_feature){ g_implicit_feature__shader_atomic_float_features, lake_arraysize(g_implicit_feature__shader_atomic_float_features), moon_implicit_feature_shader_atomic_float},
    (struct device_feature){ g_implicit_feature__shader_atomic_int64_features, lake_arraysize(g_implicit_feature__shader_atomic_int64_features), moon_implicit_feature_shader_atomic_int64},
    (struct device_feature){ g_implicit_feature__shader_float16_features, lake_arraysize(g_implicit_feature__shader_float16_features), moon_implicit_feature_shader_float16},
    (struct device_feature){ g_implicit_feature__shader_int16_features, lake_arraysize(g_implicit_feature__shader_int16_features), moon_implicit_feature_shader_int16},
    (struct device_feature){ g_implicit_feature__shader_int8_features, lake_arraysize(g_implicit_feature__shader_int8_features), moon_implicit_feature_shader_int8},
    (struct device_feature){ g_implicit_feature__dynamic_state_features, lake_arraysize(g_implicit_feature__dynamic_state_features), moon_implicit_feature_dynamic_state},
    (struct device_feature){ g_implicit_feature__sparse_binding_features, lake_arraysize(g_implicit_feature__sparse_binding_features), moon_implicit_feature_sparse_binding},
    (struct device_feature){ g_implicit_feature__swapchain_features, lake_arraysize(g_implicit_feature__swapchain_features), moon_implicit_feature_swapchain},
};
static constexpr u32 g_explicit_feature__buffer_device_address_capture_replay_features[] = {
    lake_offsetof(struct physical_device_features, buffer_device_address.bufferDeviceAddressCaptureReplay),
};
static constexpr u32 g_explicit_feature__acceleration_structure_capture_replay_features[] = {
    lake_offsetof(struct physical_device_features, acceleration_structure.accelerationStructure),
    lake_offsetof(struct physical_device_features, acceleration_structure.accelerationStructureCaptureReplay),
    lake_offsetof(struct physical_device_features, acceleration_structure.descriptorBindingAccelerationStructureUpdateAfterBind),
    lake_offsetof(struct physical_device_features, ray_query.rayQuery),
};
static constexpr u32 g_explicit_feature__vulkan_memory_model_features[] = {
    lake_offsetof(struct physical_device_features, vulkan_memory_model.vulkanMemoryModel),
    lake_offsetof(struct physical_device_features, vulkan_memory_model.vulkanMemoryModelDeviceScope),
};
static constexpr u32 g_explicit_feature__robust_access_features[] = {
    lake_offsetof(struct physical_device_features, robustness2.robustBufferAccess2),
    lake_offsetof(struct physical_device_features, robustness2.robustImageAccess2),
    lake_offsetof(struct physical_device_features, features2.features.robustBufferAccess),
};
static constexpr u32 g_explicit_feature__video_decode_queue_features[] = {
    lake_offsetof(struct physical_device_features, has_video_decode_queue),
};
static constexpr u32 g_explicit_feature__video_encode_queue_features[] = {
    lake_offsetof(struct physical_device_features, has_video_encode_queue),
};
static constexpr u32 g_explicit_feature__multiview_xr_features[] = {
    lake_offsetof(struct physical_device_features, multiview.multiview),
    lake_offsetof(struct physical_device_features, multiview.multiviewTessellationShader),
    lake_offsetof(struct physical_device_features, features2.features.multiViewport),
};
static constexpr struct device_feature g_explicit_features[] = {
    (struct device_feature){ g_explicit_feature__buffer_device_address_capture_replay_features, lake_arraysize(g_explicit_feature__buffer_device_address_capture_replay_features), moon_explicit_feature_buffer_device_address_capture_replay},
    (struct device_feature){ g_explicit_feature__acceleration_structure_capture_replay_features, lake_arraysize(g_explicit_feature__acceleration_structure_capture_replay_features), moon_explicit_feature_acceleration_structure_capture_replay},
    (struct device_feature){ g_explicit_feature__vulkan_memory_model_features, lake_arraysize(g_explicit_feature__vulkan_memory_model_features), moon_explicit_feature_vulkan_memory_model},
    (struct device_feature){ g_explicit_feature__robust_access_features, lake_arraysize(g_explicit_feature__robust_access_features), moon_explicit_feature_robust_access},
    (struct device_feature){ g_explicit_feature__video_decode_queue_features, lake_arraysize(g_explicit_feature__video_decode_queue_features), moon_explicit_feature_video_decode_queue},
    (struct device_feature){ g_explicit_feature__video_encode_queue_features, lake_arraysize(g_explicit_feature__video_encode_queue_features), moon_explicit_feature_video_encode_queue},
    (struct device_feature){ g_explicit_feature__multiview_xr_features, lake_arraysize(g_explicit_feature__multiview_xr_features), moon_explicit_feature_multiview_xr},
};
/* TODO video features */

static void query_physical_device_properties(struct physical_device_properties *properties, u64 extension_bits)
{
    void *chain = NULL;
    if (extension_bits & device_extension_amdx_shader_enqueue) {
        properties->shader_enqueue.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ENQUEUE_PROPERTIES_AMDX;
        properties->shader_enqueue.pNext = chain;
        chain = &properties->shader_enqueue;
    }
    if (extension_bits & device_extension_khr_acceleration_structure) {
        properties->acceleration_structure.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
        properties->acceleration_structure.pNext = chain;
        chain = &properties->acceleration_structure;
    }
    if (extension_bits & device_extension_khr_ray_tracing_pipeline) {
        properties->ray_tracing_pipeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
        properties->ray_tracing_pipeline.pNext = chain;
        chain = &properties->ray_tracing_pipeline;
    }
    if (extension_bits & device_extension_nv_ray_tracing_invocation_reorder) {
        properties->ray_tracing_invocation_reorder.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_PROPERTIES_NV;
        properties->ray_tracing_invocation_reorder.pNext = chain;
        chain = &properties->ray_tracing_invocation_reorder;
    }
    if (extension_bits & device_extension_khr_fragment_shading_rate) {
        properties->fragment_shading_rate.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
        properties->fragment_shading_rate.pNext = chain;
        chain = &properties->fragment_shading_rate;
    }
    if (extension_bits & device_extension_ext_mesh_shader) {
        properties->mesh_shader.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT;
        properties->mesh_shader.pNext = chain;
        chain = &properties->mesh_shader;
    }
    if (extension_bits & device_extension_ext_robustness2) {
        properties->robustness2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT;
        properties->robustness2.pNext = chain;
        chain = &properties->robustness2;
    }
    properties->descriptor_indexing.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
    properties->descriptor_indexing.pNext = chain;
    chain = &properties->descriptor_indexing;

    properties->subgroup_size_control.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES;
    properties->subgroup_size_control.pNext = chain;
    chain = &properties->subgroup_size_control;

    properties->timeline_semaphore.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES;
    properties->timeline_semaphore.pNext = chain;
    chain = &properties->timeline_semaphore;

    if (extension_bits & device_extension_khr_maintenance6) {
        properties->maintenance6.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_PROPERTIES;
        properties->maintenance6.pNext = chain;
        chain = &properties->maintenance6;
    }
    if (extension_bits & device_extension_khr_maintenance5) {
        properties->maintenance5.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_PROPERTIES;
        properties->maintenance5.pNext = chain;
        chain = &properties->maintenance5;
    }
    if (extension_bits & device_extension_khr_maintenance4) {
        properties->maintenance4.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES;
        properties->maintenance4.pNext = chain;
        chain = &properties->maintenance4;
    }
    properties->properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    properties->properties2.pNext = chain;

    chain = NULL;
    if (extension_bits & device_extension_ext_memory_budget) {
        properties->memory_budget.pNext = chain;
        properties->memory_budget.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
        chain = &properties->memory_budget;
    }
    properties->memory_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    properties->memory_properties2.pNext = chain;
}

static void query_physical_device_features(struct physical_device_features *features, u64 extension_bits)
{
    void *chain = NULL;
    if (extension_bits & device_extension_amdx_shader_enqueue) {
        features->shader_enqueue.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ENQUEUE_FEATURES_AMDX;
        features->shader_enqueue.pNext = chain;
        chain = &features->shader_enqueue;
    }
    if (extension_bits & device_extension_khr_acceleration_structure) {
        features->acceleration_structure.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        features->acceleration_structure.pNext = chain;
        chain = &features->acceleration_structure;
    }
    if (extension_bits & device_extension_khr_ray_tracing_pipeline) {
        features->ray_tracing_pipeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        features->ray_tracing_pipeline.pNext = chain;
        chain = &features->ray_tracing_pipeline;
    }
    if (extension_bits & device_extension_khr_ray_tracing_position_fetch) {
        features->ray_tracing_position_fetch.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_POSITION_FETCH_FEATURES_KHR;
        features->ray_tracing_position_fetch.pNext = chain;
        chain = &features->ray_tracing_position_fetch;
    }
    if (extension_bits & device_extension_nv_ray_tracing_invocation_reorder) {
        features->ray_tracing_invocation_reorder.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_NV;
        features->ray_tracing_invocation_reorder.pNext = chain;
        chain = &features->ray_tracing_invocation_reorder;
    }
    if (extension_bits & device_extension_khr_ray_query) {
        features->ray_query.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
        features->ray_query.pNext = chain;
        chain = &features->ray_query;
    }
    if (extension_bits & device_extension_khr_fragment_shading_rate) {
        features->fragment_shading_rate.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
        features->fragment_shading_rate.pNext = chain;
        chain = &features->fragment_shading_rate;
    }
    if (extension_bits & device_extension_ext_mesh_shader) {
        features->mesh_shader.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
        features->mesh_shader.pNext = chain;
        chain = &features->mesh_shader;
    }
    if (extension_bits & device_extension_ext_robustness2) {
        features->robustness2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
        features->robustness2.pNext = chain;
        chain = &features->robustness2;
    }
    features->descriptor_indexing.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    features->descriptor_indexing.pNext = chain;
    chain = &features->descriptor_indexing;

    if (extension_bits & device_extension_ext_shader_image_atomic_int64) {
        features->shader_image_atomic_int64.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT;
        features->shader_image_atomic_int64.pNext = chain;
        chain = &features->shader_image_atomic_int64;
    }
    if (extension_bits & device_extension_ext_shader_atomic_float) {
        features->shader_atomic_float.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
        features->shader_atomic_float.pNext = chain;
        chain = &features->shader_atomic_float;
    }
    features->shader_atomic_int64.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES;
    features->shader_atomic_int64.pNext = chain;
    chain = &features->shader_atomic_int64;

    features->shader_float16_int8.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES;
    features->shader_float16_int8.pNext = chain;
    chain = &features->shader_float16_int8;

    features->vulkan_memory_model.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES;
    features->vulkan_memory_model.pNext = chain;
    chain = &features->vulkan_memory_model;

    features->bit16_storage.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR;
    features->bit16_storage.pNext = chain;
    chain = &features->bit16_storage;

    features->bit8_storage.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR;
    features->bit8_storage.pNext = chain;
    chain = &features->bit8_storage;

    if (extension_bits & device_extension_ext_extended_dynamic_state3) {
        features->extended_dynamic_state3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT;
        features->extended_dynamic_state3.pNext = chain;
        chain = &features->extended_dynamic_state3;
    }
    features->variable_pointer.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES;
    features->variable_pointer.pNext = chain;
    chain = &features->variable_pointer;

    features->host_query_reset.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES;
    features->host_query_reset.pNext = chain;
    chain = &features->host_query_reset;

    features->buffer_device_address.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    features->buffer_device_address.pNext = chain;
    chain = &features->buffer_device_address;

    features->scalar_block_layout.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES;
    features->scalar_block_layout.pNext = chain;
    chain = &features->scalar_block_layout;

    features->subgroup_size_control.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES;
    features->subgroup_size_control.pNext = chain;
    chain = &features->subgroup_size_control;

    features->multiview.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;
    features->multiview.pNext = chain;
    chain = &features->multiview;

    features->timeline_semaphore.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;
    features->timeline_semaphore.pNext = chain;
    chain = &features->timeline_semaphore;

    features->synchronization2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    features->synchronization2.pNext = chain;
    chain = &features->synchronization2;

    if (extension_bits & device_extension_ext_dynamic_rendering_unused_attachments) {
        features->dynamic_rendering_unused_attachments.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT;
        features->dynamic_rendering_unused_attachments.pNext = chain;
        chain = &features->dynamic_rendering_unused_attachments;
    }
    if (extension_bits & device_extension_khr_dynamic_rendering_local_read) {
        features->dynamic_rendering_local_read.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES;
        features->dynamic_rendering_local_read.pNext = chain;
        chain = &features->dynamic_rendering_local_read;
    }
    features->dynamic_rendering.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    features->dynamic_rendering.pNext = chain;
    chain = &features->dynamic_rendering;

    if (extension_bits & device_extension_khr_maintenance6) {
        features->maintenance6.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_PROPERTIES;
        features->maintenance6.pNext = chain;
        chain = &features->maintenance6;
    }
    if (extension_bits & device_extension_khr_maintenance5) {
        features->maintenance5.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_PROPERTIES;
        features->maintenance5.pNext = chain;
        chain = &features->maintenance5;
    }
    if (extension_bits & device_extension_khr_maintenance4) {
        features->maintenance4.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES;
        features->maintenance4.pNext = chain;
        chain = &features->maintenance4;
    }
    features->features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features->features2.pNext = chain;

    features->has_swapchain = (extension_bits & device_extension_khr_swapchain) ? VK_TRUE : VK_FALSE;
    features->has_conservative_rasterization = (extension_bits & device_extension_ext_conservative_rasterization) ? VK_TRUE : VK_FALSE;
    features->has_video_decode_queue = (extension_bits & device_extension_khr_video_decode_queue) ? VK_TRUE : VK_FALSE;
    features->has_video_encode_queue = (extension_bits & device_extension_khr_video_encode_queue) ? VK_TRUE : VK_FALSE;
}

static void query_physical_device_details(
    moon_device_details          *write,
    struct physical_device const *pd)
{
    struct physical_device_properties const *p = &pd->vk_properties;
    struct physical_device_features const *f = &pd->vk_features;
    u8 const *raw = (u8 const *)&f;

#define QUERY_FEATURES_ARRAY(T, g_array, compare) \
    for (u32 i = 0; i < lake_arraysize(g_array); i++) { \
        struct device_feature const *feat = &g_array[i]; \
        bool all_set = true; \
        for (u32 j = 0; j < feat->offsets_count; j++) { \
            u8 const value = *(raw + feat->vk_feature_offsets[j]); \
            all_set = all_set && (value == VK_TRUE); \
        } \
        if (compare) write->T##s |= (moon_##T##_bits)feat->feature_bit; \
    }
    QUERY_FEATURES_ARRAY(missing_required_feature, g_required_features, !all_set);
    QUERY_FEATURES_ARRAY(implicit_feature, g_implicit_features, all_set);
    QUERY_FEATURES_ARRAY(explicit_feature, g_explicit_features, all_set);
#undef QUERY_FEATURES_ARRAY

    write->api_version = p->properties2.properties.apiVersion;
    write->driver_version = p->properties2.properties.driverVersion;
    write->vendor_id = p->properties2.properties.vendorID;
    write->device_id = p->properties2.properties.deviceID;
    switch (p->properties2.properties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            write->device_type = moon_device_type_integrated_gpu; break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            write->device_type = moon_device_type_discrete_gpu; break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            write->device_type = moon_device_type_virtual_gpu; break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            write->device_type = moon_device_type_cpu; break;
        default:
            write->device_type = moon_device_type_other; break;
    }
    lake_strncpy(write->device_name, p->properties2.properties.deviceName, 256u);
    lake_memcpy(write->pipeline_cache_uuid, p->properties2.properties.pipelineCacheUUID, 16u);
    write->compute_queue_count = pd->queue_families[moon_queue_type_compute].queue_count;
    write->transfer_queue_count = pd->queue_families[moon_queue_type_transfer].queue_count;

    write->heap_memory_type_count = p->memory_properties2.memoryProperties.memoryTypeCount;
    for (u32 i = 0; i < p->memory_properties2.memoryProperties.memoryTypeCount; i++) {
        write->heap_memory_types[i].heap_idx = p->memory_properties2.memoryProperties.memoryTypes[i].heapIndex;
        write->heap_memory_types[i].property = p->memory_properties2.memoryProperties.memoryTypes[i].propertyFlags;
    }

    write->heap_count = p->memory_properties2.memoryProperties.memoryHeapCount;
    for (u32 i = 0; i < p->memory_properties2.memoryProperties.memoryHeapCount; i++)
        write->heap_sizes[i] = p->memory_properties2.memoryProperties.memoryHeaps[write->heap_memory_types[i].heap_idx].size;

    /* device limits are kept ABI compatible with VkPhysicalDeviceLimits. */
    lake_memcpy(&write->limits, &p->properties2.properties.limits, sizeof(moon_device_limits));

    /* moon_device_work_graph_details */
    if (write->implicit_features & moon_implicit_feature_work_graph) {
        moon_device_work_graph_details *o = &write->work_graph_details;

        o->work_graph_dispatch_address_alignment = p->shader_enqueue.executionGraphDispatchAddressAlignment;
        o->max_work_graph_depth = p->shader_enqueue.maxExecutionGraphDepth;
        o->max_work_graph_shader_output_nodes = p->shader_enqueue.maxExecutionGraphShaderOutputNodes;
        o->max_work_graph_shader_payload_size = p->shader_enqueue.maxExecutionGraphShaderPayloadSize;
        o->max_work_graph_shader_payload_count = p->shader_enqueue.maxExecutionGraphShaderPayloadCount;
        for (u32 i = 0; i < 3; i++) 
            o->max_work_graph_workgroup_count[i] = p->shader_enqueue.maxExecutionGraphWorkgroupCount[i];
        o->max_work_graph_workgroups = p->shader_enqueue.maxExecutionGraphWorkgroups;
    }

    /* moon_device_mesh_shader_details */
    if (write->implicit_features & moon_implicit_feature_mesh_shader) {
        moon_device_mesh_shader_details *o = &write->mesh_shader_details;

        for (u32 i = 0; i < 3; i++) {
            o->max_task_work_group_count[i] = p->mesh_shader.maxTaskWorkGroupCount[i];
            o->max_task_work_group_size[i] = p->mesh_shader.maxTaskWorkGroupSize[i];
            o->max_mesh_work_group_count[i] = p->mesh_shader.maxMeshWorkGroupCount[i];
            o->max_mesh_work_group_size[i] = p->mesh_shader.maxMeshWorkGroupSize[i];
        }
        o->mesh_output_per_vertex_granularity = p->mesh_shader.meshOutputPerVertexGranularity;
        o->mesh_output_per_primitive_granularity = p->mesh_shader.meshOutputPerPrimitiveGranularity;
        o->max_task_work_group_total_count = p->mesh_shader.maxTaskWorkGroupTotalCount;
        o->max_task_work_group_invocations = p->mesh_shader.maxTaskWorkGroupInvocations;
        o->max_task_payload_size = p->mesh_shader.maxTaskPayloadSize;
        o->max_task_payload_and_shared_memory_size = p->mesh_shader.maxTaskPayloadAndSharedMemorySize;
        o->max_task_shared_memory_size = p->mesh_shader.maxTaskSharedMemorySize;
        o->max_mesh_work_group_total_count = p->mesh_shader.maxMeshWorkGroupTotalCount;
        o->max_mesh_work_group_invocations = p->mesh_shader.maxMeshWorkGroupInvocations;
        o->max_mesh_payload_and_shared_memory_size = p->mesh_shader.maxMeshPayloadAndSharedMemorySize;
        o->max_mesh_payload_and_output_memory_size = p->mesh_shader.maxMeshPayloadAndOutputMemorySize;
        o->max_mesh_shared_memory_size = p->mesh_shader.maxMeshSharedMemorySize;
        o->max_mesh_output_memory_size = p->mesh_shader.maxMeshOutputMemorySize;
        o->max_mesh_output_components = p->mesh_shader.maxMeshOutputComponents;
        o->max_mesh_output_vertices = p->mesh_shader.maxMeshOutputVertices;
        o->max_mesh_output_primitives = p->mesh_shader.maxMeshOutputPrimitives;
        o->max_mesh_output_layers = p->mesh_shader.maxMeshOutputLayers;
        o->max_mesh_multiview_view_count = p->mesh_shader.maxMeshMultiviewViewCount;
        o->max_preferred_task_work_group_invocations = p->mesh_shader.maxPreferredTaskWorkGroupInvocations;
        o->max_preferred_mesh_work_group_invocations = p->mesh_shader.maxPreferredMeshWorkGroupInvocations;
        o->prefers_local_invocation_vertex_output = p->mesh_shader.prefersLocalInvocationVertexOutput;
        o->prefers_local_invocation_primitive_output = p->mesh_shader.prefersLocalInvocationPrimitiveOutput;
    }

    /* moon_device_acceleration_structure_details */
    if (write->implicit_features & moon_implicit_feature_basic_ray_tracing) {
        moon_device_acceleration_structure_details *o = &write->acceleration_structure_details;

        o->max_geometry_count = p->acceleration_structure.maxGeometryCount;
        o->max_instance_count = p->acceleration_structure.maxInstanceCount;
        o->max_primitive_count = p->acceleration_structure.maxPrimitiveCount;
        o->max_per_stage_descriptor_acceleration_structures = p->acceleration_structure.maxPerStageDescriptorAccelerationStructures;
        o->max_per_stage_descriptor_update_after_bind_acceleration_structures = p->acceleration_structure.maxPerStageDescriptorUpdateAfterBindAccelerationStructures;
        o->max_descriptor_set_acceleration_structures = p->acceleration_structure.maxDescriptorSetAccelerationStructures;
        o->max_descriptor_set_update_after_bind_acceleration_structures = p->acceleration_structure.maxDescriptorSetUpdateAfterBindAccelerationStructures;
        o->min_acceleration_structure_scratch_offset_alignment = p->acceleration_structure.minAccelerationStructureScratchOffsetAlignment;
    }

    /* moon_device_ray_tracing_pipeline_details */
    if (write->implicit_features & moon_implicit_feature_ray_tracing_pipeline) {
        moon_device_ray_tracing_pipeline_details *o = &write->ray_tracing_pipeline_details;

        o->shader_group_handle_size = p->ray_tracing_pipeline.shaderGroupHandleSize;
        o->shader_group_handle_capture_replay_size = p->ray_tracing_pipeline.shaderGroupHandleCaptureReplaySize;
        o->shader_group_handle_alignment = p->ray_tracing_pipeline.shaderGroupHandleAlignment;
        o->shader_group_base_alignment = p->ray_tracing_pipeline.shaderGroupBaseAlignment;
        o->max_shader_group_stride = p->ray_tracing_pipeline.maxShaderGroupStride;
        o->max_ray_recursion_depth = p->ray_tracing_pipeline.maxRayRecursionDepth;
        o->max_ray_dispatch_invocation_count = p->ray_tracing_pipeline.maxRayDispatchInvocationCount;
        o->max_ray_hit_attribute_size = p->ray_tracing_pipeline.maxRayHitAttributeSize;
        o->invocation_reorder_hint = p->ray_tracing_invocation_reorder.rayTracingInvocationReorderReorderingHint;
    }

    /* TODO calculate a detailed score i guess */
    write->total_score = 5;
    if (write->device_type == moon_device_type_discrete_gpu)
        write->total_score = 10000;
    else if (write->device_type == moon_device_type_integrated_gpu)
        write->total_score = 5000;
    else if (write->device_type == moon_device_type_cpu)
        write->total_score = 500;
    else if (write->device_type == moon_device_type_virtual_gpu)
        write->total_score = 50;
}

struct query_physical_device_work {
    u32                     idx;
    u32                     pd_count;
    char const             *name;
    moon_adapter            moon;
    struct physical_device  write;
};

static FN_LAKE_WORK(query_physical_device, struct query_physical_device_work *work)
{
    char const *name = work->name;
    moon_adapter const moon = work->moon;
    struct physical_device *write = &work->write;

    VkQueueFamilyVideoPropertiesKHR *queue_family_video_properties;
    VkQueueFamilyProperties2 *queue_family_properties2;
    VkExtensionProperties *extension_properties;
    char const *pd_name;
    u32 pd_api_version;
    u32 queue_family_count;
    u32 extension_count;

    moon->vkGetPhysicalDeviceProperties(write->vk_physical_device, &write->vk_properties.properties2.properties);
    moon->vkGetPhysicalDeviceQueueFamilyProperties2(write->vk_physical_device, &queue_family_count, nullptr);
    VERIFY_VK_ERROR(moon->vkEnumerateDeviceExtensionProperties(write->vk_physical_device, nullptr, &extension_count, nullptr));
    pd_name = write->vk_properties.properties2.properties.deviceName;
    pd_api_version = write->vk_properties.properties2.properties.apiVersion;

    /* early checks to invalidate faulty or too old physical devices */
    if (pd_api_version < VK_API_VERSION_1_2) {
        lake_dbg_2("%s: DISMISSED physical device (%u of %u) `%s` "
            " has too old drivers. Found Vulkan API version %u.%u.%u, we target atleast 1.2.X.",
            name, work->idx, work->pd_count, pd_name, (pd_api_version >> 22u),
            (pd_api_version >> 12u) & 0x3ffu, (pd_api_version & 0xfffu));
        return;
    } else if (queue_family_count == 0) {
        lake_dbg_2("%s: DISMISSED physical device (%u of %u) `%s` has no queue families.", name, work->idx, work->pd_count, pd_name);
        return;
    } else if (extension_count == 0) {
        lake_dbg_2("%s: DISMISSED physical device (%u of %u) `%s` has no Vulkan extension support.", name, work->idx, work->pd_count, pd_name);
        return;
    }

    u8 *raw = nullptr; 
    { /* get scratch memory */
        usize const queue_family_video_properties_bytes = lake_align(sizeof(VkQueueFamilyVideoPropertiesKHR) * queue_family_count, 16);
        usize const queue_family_properties2_bytes = lake_align(sizeof(VkQueueFamilyProperties2) * queue_family_count, 16);
        usize const extension_properties_bytes = lake_align(sizeof(VkExtensionProperties) * extension_count, 16);
        usize const total_bytes = 
            queue_family_video_properties_bytes +
            queue_family_properties2_bytes +
            extension_properties_bytes;

        usize o = 0;
        raw = (u8 *)__lake_malloc(total_bytes, 16);

        queue_family_video_properties = (VkQueueFamilyVideoPropertiesKHR *)&raw[o];
        o += queue_family_video_properties_bytes;
        queue_family_properties2 = (VkQueueFamilyProperties2 *)&raw[o];
        o += queue_family_properties2_bytes;
        extension_properties = (VkExtensionProperties *)&raw[o];
        lake_san_assert(o + extension_properties_bytes == total_bytes, LAKE_VALIDATION_FAILED, nullptr);
    }

    /* resolve queue families */
    for (u32 i = 0; i < queue_family_count; i++) {
        queue_family_properties2[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        queue_family_properties2[i].pNext = &queue_family_video_properties[i];
        queue_family_video_properties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_VIDEO_PROPERTIES_KHR;
        queue_family_video_properties[i].pNext = nullptr;
        queue_family_video_properties[i].videoCodecOperations = 0;
        write->queue_families[i].vk_index = -1;
    }
    moon->vkGetPhysicalDeviceQueueFamilyProperties2(write->vk_physical_device, &queue_family_count, queue_family_properties2);

    u32 found_queue_families = 0u;
    /* search for specialized command queue families */
    for (u32 i = 0; i < queue_family_count; i++) {
        VkQueueFlags flags = queue_family_properties2[i].queueFamilyProperties.queueFlags;
        u32 queue_count = queue_family_properties2[i].queueFamilyProperties.queueCount;
        if (queue_count == 0) continue;

        /* don't be picky about the main queue */
        if (!(found_queue_families & (1u << moon_queue_type_main)) && flags & VK_QUEUE_GRAPHICS_BIT) {
            write->queue_families[moon_queue_type_main].queue_count = queue_count;
            write->queue_families[moon_queue_type_main].vk_index = i;
            write->main_queue_command_support = flags;
            found_queue_families |= (1u << moon_queue_type_main);

        /* try for an async compute family */
        } else if (!(found_queue_families & (1u << moon_queue_type_compute)) && VK_QUEUE_COMPUTE_BIT
            == (flags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT)))
        {
            write->queue_families[moon_queue_type_compute].queue_count = queue_count;
            write->queue_families[moon_queue_type_compute].vk_index = i;
            found_queue_families |= (1u << moon_queue_type_compute);

        /* try for an async transfer family */
        } else if (!(found_queue_families & (1u << moon_queue_type_transfer)) && VK_QUEUE_TRANSFER_BIT
            == (flags & (VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT)))
        {
            write->queue_families[moon_queue_type_transfer].queue_count = queue_count;
            write->queue_families[moon_queue_type_transfer].vk_index = i;
            found_queue_families |= (1u << moon_queue_type_transfer);

        /* try for an async sparse binding family */
        } else if (!(found_queue_families & (1u << moon_queue_type_sparse_binding)) && VK_QUEUE_SPARSE_BINDING_BIT
            == (flags & (VK_QUEUE_SPARSE_BINDING_BIT | VK_QUEUE_GRAPHICS_BIT)))
        {
            write->queue_families[moon_queue_type_sparse_binding].queue_count = queue_count;
            write->queue_families[moon_queue_type_sparse_binding].vk_index = i;
            found_queue_families |= (1u << moon_queue_type_sparse_binding);

        /* try for an async video decode queue */
        } else if (!(found_queue_families & (1u << moon_queue_type_video_decode)) && VK_QUEUE_VIDEO_DECODE_BIT_KHR
            == (flags & (VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_GRAPHICS_BIT)))
        {
            write->queue_families[moon_queue_type_video_decode].queue_count = queue_count;
            write->queue_families[moon_queue_type_video_decode].vk_index = i;
            found_queue_families |= (1u << moon_queue_type_video_decode);

        /* try for an async video encode queue */
        } else if (!(found_queue_families & (1u << moon_queue_type_video_encode)) && VK_QUEUE_VIDEO_ENCODE_BIT_KHR
            == (flags & (VK_QUEUE_VIDEO_ENCODE_BIT_KHR | VK_QUEUE_GRAPHICS_BIT)))
        {
            write->queue_families[moon_queue_type_video_encode].queue_count = queue_count;
            write->queue_families[moon_queue_type_video_encode].vk_index = i;
            found_queue_families |= (1u << moon_queue_type_video_encode);
        }
    }
    if (!(found_queue_families & (1u << moon_queue_type_main))) {
        lake_dbg_2("%s: DISMISSED physical device (%u of %u) `%s` has no graphics command support.", name, work->idx, work->pd_count, pd_name);
        return;
    }
    write->found_queue_families = found_queue_families;

    /* resolve device extensions */
    for (u32 i = 0; i < device_extension_count; i++)
        if (query_extension(extension_properties, extension_count, g_device_extension_names[i]))
            write->extension_bits |= (1llu << i);
    if (pd_api_version < VK_API_VERSION_1_4)
        for (u32 i = device_extension_count; i < device_extension_count_1_4_fallback; i++)
            if (query_extension(extension_properties, extension_count, g_device_extension_names[i]))
                write->extension_bits |= (1llu << i);
    if (pd_api_version < VK_API_VERSION_1_3)
        for (u32 i = device_extension_count_1_4_fallback; i < device_extension_count_1_3_fallback; i++)
            if (query_extension(extension_properties, extension_count, g_device_extension_names[i]))
                write->extension_bits |= (1llu << i);
    __lake_free(raw);

    /* fill Vulkan physical device info */
    query_physical_device_properties(&write->vk_properties, write->extension_bits);
    query_physical_device_features(&write->vk_features, write->extension_bits);
    /* TODO video */

    moon->vkGetPhysicalDeviceProperties2(write->vk_physical_device, &write->vk_properties.properties2);
    moon->vkGetPhysicalDeviceMemoryProperties2(write->vk_physical_device, &write->vk_properties.memory_properties2);
    moon->vkGetPhysicalDeviceFeatures2(write->vk_physical_device, &write->vk_features.features2);

    /* we accept this device */
    query_physical_device_details(&write->details, write);
}

FN_MOON_LIST_DEVICE_DETAILS(vulkan)
{
    lake_dbg_assert(moon && out_device_count, LAKE_INVALID_PARAMETERS, nullptr);
    s32 const pd_count = moon->physical_devices.da.size;

    if (out_details) {
        lake_san_assert(*out_device_count >= moon->physical_devices.da.size, LAKE_INVALID_PARAMETERS, 
            "The count query from user is smaller than the internal device count.");
        for (s32 i = 0; i < pd_count; i++)
            out_details[i] = &moon->physical_devices.v[i].details;
    }
    *out_device_count = pd_count;
}

static moon_adapter g_moon = nullptr;

static FN_LAKE_WORK(moon_interface_destructor, moon_adapter moon)
{
    if (moon == nullptr)
        return;

    s32 refcnt = lake_atomic_read(&moon->interface.header.refcnt);
    lake_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, nullptr);

    if (moon->physical_devices.v)
        __lake_free(moon->physical_devices.v); /* TODO darray */

    if (moon->vk_instance)
        moon->vkDestroyInstance(moon->vk_instance, &moon->vk_allocator);
    if (moon->vulkan_library)
        lake_close_library(moon->vulkan_library);
    __lake_free(moon);
    g_moon = nullptr;
}

LAKEAPI FN_LAKE_WORK(moon_interface_assembly_vulkan, moon_interface_assembly const *assembly)
{
    char const *name = "moon/vulkan";
    void *vulkan_library;
    u32 api_version = 0;
    u32 layer_count = 0;
    u32 extension_count = 0;
    u32 extension_bits = 0;
    u32 physical_device_count = 0;
    VkAllocationCallbacks const vk_allocator = {
        .pfnAllocation = handle_allocation_callback__malloc,
        .pfnReallocation = handle_allocation_callback__realloc,
        .pfnFree = handle_allocation_callback__free,
        .pfnInternalAllocation = nullptr,
        .pfnInternalFree = nullptr,
        .pUserData = nullptr,
    };

    /* we allow only one Vulkan backend instance to exist at a time. */
    if (lake_unlikely(g_moon != nullptr)) {
        lake_refcnt_inc(&g_moon->interface.header.refcnt);
        assembly->out_impl->adapter = g_moon;
        return;
    }

#if defined(LAKE_PLATFORM_WINDOWS)
    vulkan_library = lake_open_library("vulkan-1.dll");
#elif defined(LAKE_PLATFORM_APPLE)
    vulkan_library = lake_open_library("libvulkan.dylib");
    if (vulkan_library == nullptr)
        vulkan_library = lake_open_library("libvulkan.1.dylib");
    if (vulkan_library == nullptr)
        vulkan_library = lake_open_library("libMoltenVK.dylib");
    /* Add support for using Vulkan and MoltenVK in a framework. App store rules for iOS 
     * strictly enforce no .dylib's. If they aren't found it just falls through. */
    if (vulkan_library == nullptr)
        vulkan_library = lake_open_library("vulkan.framework/vulkan");
    if (vulkan_library == nullptr)
        vulkan_library = lake_open_library("MoltenVK.framework/MoltenVK");
    /* Modern versions of MacOS don't search /usr/local/lib automatically contrary to what
     * man dlopen says. Vulkan SDK uses this as the system-wide installation location, so 
     * we're going to fallback to this if all else fails. */
    if (vulkan_library == nullptr)
        vulkan_library = lake_open_library("/usr/local/lib/libvulkan.dylib");
#else
    vulkan_library = lake_open_library("libvulkan.so.1");
    if (vulkan_library == nullptr)
        vulkan_library = lake_open_library("libvulkan.so");
#endif
    if (vulkan_library == nullptr) {
        lake_dbg_1("%s: can't open the Vulkan drivers, ensure they are correclty installed and available via the system PATH.", name);
        return;
    }

    char const *vkGetInstanceProcAddr_name = "vkGetInstanceProcAddr";
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)lake_get_proc_address(vulkan_library, vkGetInstanceProcAddr_name);
    if (vkGetInstanceProcAddr == nullptr) {
        lake_dbg_1("%s: can't get the address of %s from Vulkan drivers.", name, vkGetInstanceProcAddr_name);
        lake_close_library(vulkan_library);
        return;
    }

    PFN_vkCreateInstance vkCreateInstance = (PFN_vkCreateInstance)
        vkGetInstanceProcAddr(nullptr, "vkCreateInstance");
    PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)
        vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion");
    PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)
        vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties");
    PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)
        vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties");
    if (!vkCreateInstance || 
        !vkEnumerateInstanceVersion || 
        !vkEnumerateInstanceExtensionProperties || 
        !vkEnumerateInstanceLayerProperties) 
    {
        lake_dbg_1("%s: can't get addresses of global procedures from Vulkan drivers.", name);
        lake_close_library(vulkan_library);
        return;
    }

    /* check the API version */
    vkEnumerateInstanceVersion(&api_version);
    if (api_version < VK_API_VERSION_1_2) {
        lake_dbg_1("%s: outdated drivers. We target a minimum of Vulkan 1.2 core, "
                "your drivers API version is %u.%u.%u.",
                name, (api_version >> 22u), (api_version >> 12u) & 0x3ffu, (api_version & 0xfffu));
        lake_close_library(vulkan_library);
        return;
    }
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    /* check instance extensions */
    VERIFY_VK_ERROR(vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    if (lake_unlikely(!extension_count)) {
        lake_dbg_1("%s: no instance extensions available, can't continue.", name);
        lake_close_library(vulkan_library);
        return;
    }

    VkExtensionProperties *extension_properties = (VkExtensionProperties *)
        __lake_malloc_n(VkExtensionProperties, extension_count);
    VERIFY_VK_ERROR(vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_properties));

    /* query extensions we will be using */
    for (u32 i = 0; i < instance_extension_count; i++) 
        if (query_extension(extension_properties, extension_count, g_instance_extension_names[i]))
            extension_bits |= (1u << i);
    /* don't enable debug utils if not requested */
    if (assembly->framework->hints.use_debug_instruments <= 0)
        extension_bits &= ~(instance_extension_ext_debug_utils | instance_extension_layer_validation);
    __lake_free(extension_properties);

    extension_count = 0;
    char const *extensions[instance_extension_count];

    for (u32 i = 0; i < instance_extension_count; i++)
        if (extension_bits & (1u << i)) 
            extensions[extension_count++] = g_instance_extension_names[i];

    VkApplicationInfo vk_app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = assembly->framework->app_name,
        .applicationVersion = assembly->framework->build_app_ver,
        .pEngineName = assembly->framework->engine_name,
        .engineVersion = assembly->framework->build_engine_ver,
        .apiVersion = api_version,
    };
    VkInstanceCreateInfo vk_instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &vk_app_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = (char const * const *)extensions,
    };
#ifndef LAKE_NDEBUG
    VkValidationFeatureEnableEXT vk_validation_feature_enable[] = {
        VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
        VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
        VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
    };
    VkValidationFeaturesEXT vk_validation_features = {
        .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
        .pNext = nullptr,
        .enabledValidationFeatureCount = lake_arraysize(vk_validation_feature_enable),
        .pEnabledValidationFeatures = vk_validation_feature_enable,
        .disabledValidationFeatureCount = 0,
        .pDisabledValidationFeatures = nullptr,
    };
    char const *vk_validation_layers[] = {
        "VK_LAYER_KHRONOS_validation",
    };
    /* enable validation layers if debug requested */
    if ((extension_bits & instance_extension_ext_debug_utils) && assembly->framework->hints.use_debug_instruments) {
        extension_bits |= instance_extension_layer_validation;
        vk_instance_info.pNext = &vk_validation_features;
        vk_instance_info.enabledLayerCount = 1;
        vk_instance_info.ppEnabledLayerNames = (char const * const *)vk_validation_layers;
    }
#endif /* LAKE_NDEBUG */

    /* create the vulkan instance */
    VkInstance vk_instance = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(&vk_instance_info, &vk_allocator, &vk_instance);
    if (result != VK_SUCCESS) {
        lake_dbg_1("%s: creating a Vulkan instance failed: %s.", name, vk_result_string(result));
        lake_close_library(vulkan_library);
        return;
    }

    moon_adapter moon = __lake_malloc_t(struct moon_adapter_impl);
    lake_zerop(moon);
    g_moon = moon;

    moon->vk_instance = vk_instance;
    moon->vk_allocator = vk_allocator;
    moon->vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    moon->vkCreateInstance = vkCreateInstance;
    moon->vkEnumerateInstanceVersion = vkEnumerateInstanceVersion;
    moon->vkEnumerateInstanceExtensionProperties = vkEnumerateInstanceExtensionProperties;
    moon->vkEnumerateInstanceLayerProperties = vkEnumerateInstanceLayerProperties;
    moon->vulkan_library = vulkan_library;
    moon->api_version = api_version;

    /* write the interface header */
    moon->interface.header.framework = assembly->framework;
    moon->interface.header.destructor = (PFN_lake_work)moon_interface_destructor;
    moon->interface.header.name.len = lake_strlen(name) + 1;
    lake_memcpy(moon->interface.header.name.str, name, moon->interface.header.name.len);

    if (lake_unlikely(!load_vk_instance_symbols(moon, extension_bits))) {
        moon_interface_destructor(moon);
        return;
    }
#ifndef LAKE_NDEBUG
    if (extension_bits & instance_extension_layer_validation) {
        VkDebugUtilsMessengerCreateInfoEXT messenger_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = handle_debug_utils_callback,
            .pUserData = nullptr,
        };
        result = moon->vkCreateDebugUtilsMessengerEXT(moon->vk_instance, &messenger_info, &moon->vk_allocator, &moon->vk_debug_messenger);
        if (result == VK_SUCCESS) {
            lake_dbg_2("%s: created validation layers: %s.", name, vk_validation_layers[0]);
        } else {
            lake_dbg_2("%s: creating a debug utils messenger for %s failed: %s.", name, vk_validation_layers[0], vk_result_string(result));
        }
    }
#endif /* LAKE_NDEBUG */

    /* query physical devices */
    result = moon->vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, nullptr);
    if (result != VK_SUCCESS || physical_device_count == 0) {
        lake_dbg_1("%s: no physical devices are available to Vulkan: %s.", name, vk_result_string(result));
        moon_interface_destructor(moon);
        return;
    }
    usize const vk_physical_devices_bytes = lake_align(sizeof(VkPhysicalDevice) * physical_device_count, 16);
    usize const query_physical_device_work_bytes = lake_align(sizeof(struct query_physical_device_work) * physical_device_count, 16);
    usize const query_work_bytes = lake_align(sizeof(lake_work_details) * physical_device_count, 16);
    usize const indices_bytes = lake_align(sizeof(u32) * physical_device_count, 16);
    usize const total_bytes =
        vk_physical_devices_bytes +
        query_physical_device_work_bytes +
        query_work_bytes +
        indices_bytes;

    usize o = 0;
    u8 *raw = (u8 *)__lake_malloc(total_bytes, 16);
    lake_memset(raw, 0, total_bytes);

    VkPhysicalDevice *vk_physical_devices = (VkPhysicalDevice *)raw;
    o += vk_physical_devices_bytes;
    struct query_physical_device_work *query_physical_device_work = 
        (struct query_physical_device_work *)&raw[o];
    o += query_physical_device_work_bytes;
    lake_work_details *query_work = (lake_work_details *)&raw[o];
    o += query_work_bytes;
    u32 *indices = (u32 *)&raw[o];

    lake_san_assert(o + indices_bytes == total_bytes, LAKE_VALIDATION_FAILED, nullptr);
    VERIFY_VK_ERROR(moon->vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, vk_physical_devices));

    /* execute the physical device query as a job per physical device */
    for (u32 i = 0; i < physical_device_count; i++) {
        query_physical_device_work[i].idx = i;
        query_physical_device_work[i].pd_count = physical_device_count;
        query_physical_device_work[i].moon = moon;
        query_physical_device_work[i].name = name;
        query_physical_device_work[i].write.vk_physical_device = vk_physical_devices[i];
        query_work[i].procedure = (PFN_lake_work)query_physical_device;
        query_work[i].argument = (void *)&query_physical_device_work[i];
        query_work[i].name = "moon/vulkan/query_physical_device";
    }
    lake_submit_work_and_yield(physical_device_count, query_work);

    o = 0;
    /* collect indices of accepted physical devices */
    for (u32 i = 0; i < physical_device_count; i++)
        if (query_physical_device_work[i].write.details.total_score > 0)
            indices[o++] = i;
    if (o == 0) {
        lake_dbg_1("%s: invalidated all available physical devices (%u):", name, physical_device_count);
        for (u32 i = 0; i < physical_device_count; i++)
            lake_dbg_1("    - (idx:%u) %s", i, query_physical_device_work[i].write.vk_properties.properties2.properties.deviceName);
        __lake_free(raw);
        moon_interface_destructor(moon);
        return;
    }
    /* sort indices by device score */
    for (u32 i = 0, j, max_idx; i < o; i++) {
        max_idx = i;
        for (j = i + 1; j < o; j++)
            if (query_physical_device_work[indices[j]].write.details.total_score >
                query_physical_device_work[indices[max_idx]].write.details.total_score)
            {
                max_idx = j;
            }
        lake_swap(indices[max_idx], indices[i]);
    }
    /* TODO darray */
    moon->physical_devices.v = __lake_malloc_n(struct physical_device, o);
    moon->physical_devices.da.alloc = sizeof(struct physical_device) * o;
    moon->physical_devices.da.size = o;

    for (u32 i = 0; i < o; i++) {
        struct physical_device *pd = &moon->physical_devices.v[i];
        lake_memcpy(pd, &query_physical_device_work[indices[i]].write, sizeof(struct physical_device));
    }
    __lake_free(raw);

    /* write the interface */
    moon->interface.list_device_details = _moon_vulkan_list_device_details;
    moon->interface.device_assembly = _moon_vulkan_device_assembly;
    moon->interface.device_destructor = _moon_vulkan_device_destructor;
    moon->interface.device_queue_count = _moon_vulkan_device_queue_count;
    moon->interface.device_queue_wait_idle = _moon_vulkan_device_queue_wait_idle;
    moon->interface.device_wait_idle = _moon_vulkan_device_wait_idle;
    moon->interface.device_submit_commands = _moon_vulkan_device_submit_commands;
    moon->interface.device_present_frames = _moon_vulkan_device_present_frames;
    moon->interface.device_destroy_deferred = _moon_vulkan_device_destroy_deferred;
    moon->interface.device_heap_assembly = _moon_vulkan_device_heap_assembly;
    moon->interface.device_heap_destructor = _moon_vulkan_device_heap_destructor;
    moon->interface.device_buffer_memory_requirements = _moon_vulkan_device_buffer_memory_requirements;
    moon->interface.device_texture_memory_requirements = _moon_vulkan_device_texture_memory_requirements;
    moon->interface.device_memory_report = _moon_vulkan_device_memory_report;
    moon->interface.create_buffer = _moon_vulkan_create_buffer;
    moon->interface.create_buffer_from_heap = _moon_vulkan_create_buffer_from_heap;
    moon->interface.create_texture = _moon_vulkan_create_texture;
    moon->interface.create_texture_from_heap = _moon_vulkan_create_texture_from_heap;
    moon->interface.create_texture_view = _moon_vulkan_create_texture_view;
    moon->interface.create_sampler = _moon_vulkan_create_sampler;
    moon->interface.create_tlas = _moon_vulkan_create_tlas;
    moon->interface.create_tlas_from_buffer = _moon_vulkan_create_tlas_from_buffer;
    moon->interface.create_blas = _moon_vulkan_create_blas;
    moon->interface.create_blas_from_buffer = _moon_vulkan_create_blas_from_buffer;
    moon->interface.is_buffer_valid = _moon_vulkan_is_buffer_valid;
    moon->interface.is_texture_valid = _moon_vulkan_is_texture_valid;
    moon->interface.is_texture_view_valid = _moon_vulkan_is_texture_view_valid;
    moon->interface.is_sampler_valid = _moon_vulkan_is_sampler_valid;
    moon->interface.is_tlas_valid = _moon_vulkan_is_tlas_valid;
    moon->interface.is_blas_valid = _moon_vulkan_is_blas_valid;
    moon->interface.buffer_host_address = _moon_vulkan_buffer_host_address;
    moon->interface.buffer_device_address = _moon_vulkan_buffer_device_address;
    moon->interface.tlas_device_address = _moon_vulkan_tlas_device_address;
    moon->interface.blas_device_address = _moon_vulkan_blas_device_address;
    moon->interface.destroy_buffer = _moon_vulkan_destroy_buffer;
    moon->interface.destroy_texture = _moon_vulkan_destroy_texture;
    moon->interface.destroy_texture_view = _moon_vulkan_destroy_texture_view;
    moon->interface.destroy_sampler = _moon_vulkan_destroy_sampler;
    moon->interface.destroy_tlas = _moon_vulkan_destroy_tlas;
    moon->interface.destroy_blas = _moon_vulkan_destroy_blas;
    moon->interface.timeline_query_pool_assembly = _moon_vulkan_timeline_query_pool_assembly;
    moon->interface.timeline_query_pool_destructor = _moon_vulkan_timeline_query_pool_destructor;
    moon->interface.timeline_semaphore_assembly = _moon_vulkan_timeline_semaphore_assembly;
    moon->interface.timeline_semaphore_destructor = _moon_vulkan_timeline_semaphore_destructor;
    moon->interface.binary_semaphore_assembly = _moon_vulkan_binary_semaphore_assembly;
    moon->interface.binary_semaphore_destructor = _moon_vulkan_binary_semaphore_destructor;
    moon->interface.compute_pipelines_assembly = _moon_vulkan_compute_pipelines_assembly;
    moon->interface.compute_pipeline_destructor = _moon_vulkan_compute_pipeline_destructor;
    moon->interface.work_graph_pipelines_assembly = _moon_vulkan_work_graph_pipelines_assembly;
    moon->interface.work_graph_pipeline_destructor = _moon_vulkan_work_graph_pipeline_destructor;
    moon->interface.ray_tracing_pipelines_assembly = _moon_vulkan_ray_tracing_pipelines_assembly;
    moon->interface.ray_tracing_pipeline_destructor = _moon_vulkan_ray_tracing_pipeline_destructor;
    moon->interface.raster_pipelines_assembly = _moon_vulkan_raster_pipelines_assembly;
    moon->interface.raster_pipeline_destructor = _moon_vulkan_raster_pipeline_destructor;
    moon->interface.swapchain_assembly = _moon_vulkan_swapchain_assembly;
    moon->interface.swapchain_destructor = _moon_vulkan_swapchain_destructor;
    moon->interface.swapchain_wait_for_next_frame = _moon_vulkan_swapchain_wait_for_next_frame;
    moon->interface.swapchain_acquire_next_image = _moon_vulkan_swapchain_acquire_next_image;
    moon->interface.swapchain_current_acquire_semaphore = _moon_vulkan_swapchain_current_acquire_semaphore;
    moon->interface.swapchain_current_present_semaphore = _moon_vulkan_swapchain_current_present_semaphore;
    moon->interface.swapchain_current_cpu_timeline_value = _moon_vulkan_swapchain_current_cpu_timeline_value;
    moon->interface.swapchain_current_timeline_pair = _moon_vulkan_swapchain_current_timeline_pair;
    moon->interface.swapchain_gpu_timeline_semaphore = _moon_vulkan_swapchain_gpu_timeline_semaphore;
    moon->interface.swapchain_set_present_mode = _moon_vulkan_swapchain_set_present_mode;
    moon->interface.swapchain_resize = _moon_vulkan_swapchain_resize;
    moon->interface.command_recorder_assembly = _moon_vulkan_command_recorder_assembly;
    moon->interface.command_recorder_destructor = _moon_vulkan_command_recorder_destructor;
    moon->interface.staged_command_list_assembly = _moon_vulkan_staged_command_list_assembly;
    moon->interface.staged_command_list_destructor = _moon_vulkan_staged_command_list_destructor;
    moon->interface.cmd_copy_buffer = _moon_vulkan_cmd_copy_buffer;
    moon->interface.cmd_copy_buffer_to_texture = _moon_vulkan_cmd_copy_buffer_to_texture;
    moon->interface.cmd_copy_texture_to_buffer = _moon_vulkan_cmd_copy_texture_to_buffer;
    moon->interface.cmd_copy_texture = _moon_vulkan_cmd_copy_texture;
    moon->interface.cmd_blit_texture = _moon_vulkan_cmd_blit_texture;
    moon->interface.cmd_resolve_texture = _moon_vulkan_cmd_resolve_texture;
    moon->interface.cmd_clear_buffer = _moon_vulkan_cmd_clear_buffer;
    moon->interface.cmd_clear_texture = _moon_vulkan_cmd_clear_texture;
    moon->interface.cmd_destroy_buffer_deferred = _moon_vulkan_cmd_destroy_buffer_deferred;
    moon->interface.cmd_destroy_texture_deferred = _moon_vulkan_cmd_destroy_texture_deferred;
    moon->interface.cmd_destroy_texture_view_deferred = _moon_vulkan_cmd_destroy_texture_view_deferred;
    moon->interface.cmd_destroy_sampler_deferred = _moon_vulkan_cmd_destroy_sampler_deferred;
    moon->interface.cmd_build_acceleration_structures = _moon_vulkan_cmd_build_acceleration_structures;
    moon->interface.cmd_root_constants = _moon_vulkan_cmd_root_constants;
    moon->interface.cmd_set_compute_pipeline = _moon_vulkan_cmd_set_compute_pipeline;
    moon->interface.cmd_set_work_graph_pipeline = _moon_vulkan_cmd_set_work_graph_pipeline;
    moon->interface.cmd_set_ray_tracing_pipeline = _moon_vulkan_cmd_set_ray_tracing_pipeline;
    moon->interface.cmd_set_raster_pipeline = _moon_vulkan_cmd_set_raster_pipeline;
    moon->interface.cmd_set_viewport = _moon_vulkan_cmd_set_viewport;
    moon->interface.cmd_set_scissor = _moon_vulkan_cmd_set_scissor;
    moon->interface.cmd_set_rasterization_samples = _moon_vulkan_cmd_set_rasterization_samples;
    moon->interface.cmd_set_depth_bias = _moon_vulkan_cmd_set_depth_bias;
    moon->interface.cmd_set_index_buffer = _moon_vulkan_cmd_set_index_buffer;
    moon->interface.cmd_barriers_and_transitions = _moon_vulkan_cmd_barriers_and_transitions;
    moon->interface.cmd_begin_renderpass = _moon_vulkan_cmd_begin_renderpass;
    moon->interface.cmd_end_renderpass = _moon_vulkan_cmd_end_renderpass;
    moon->interface.cmd_write_timestamps = _moon_vulkan_cmd_write_timestamps;
    moon->interface.cmd_resolve_timestamps = _moon_vulkan_cmd_resolve_timestamps;
    moon->interface.cmd_begin_label = _moon_vulkan_cmd_begin_label;
    moon->interface.cmd_end_label = _moon_vulkan_cmd_end_label;
    moon->interface.cmd_dispatch = _moon_vulkan_cmd_dispatch;
    moon->interface.cmd_dispatch_indirect = _moon_vulkan_cmd_dispatch_indirect;
    moon->interface.cmd_dispatch_graph = _moon_vulkan_cmd_dispatch_graph;
    moon->interface.cmd_dispatch_graph_indirect = _moon_vulkan_cmd_dispatch_graph_indirect;
    moon->interface.cmd_dispatch_graph_indirect_count = _moon_vulkan_cmd_dispatch_graph_indirect_count;
    moon->interface.cmd_trace_rays = _moon_vulkan_cmd_trace_rays;
    moon->interface.cmd_trace_rays_indirect = _moon_vulkan_cmd_trace_rays_indirect;
    moon->interface.cmd_draw = _moon_vulkan_cmd_draw;
    moon->interface.cmd_draw_indexed = _moon_vulkan_cmd_draw_indexed;
    moon->interface.cmd_draw_indirect = _moon_vulkan_cmd_draw_indirect;
    moon->interface.cmd_draw_indirect_count = _moon_vulkan_cmd_draw_indirect_count;
    moon->interface.cmd_draw_mesh_tasks = _moon_vulkan_cmd_draw_mesh_tasks;
    moon->interface.cmd_draw_mesh_tasks_indirect = _moon_vulkan_cmd_draw_mesh_tasks_indirect;
    moon->interface.cmd_draw_mesh_tasks_indirect_count = _moon_vulkan_cmd_draw_mesh_tasks_indirect_count;

    lake_trace("Connected %s, instance ver. %u.%u.%u, %u physical devices available.", name, 
        (api_version >> 22u), (api_version >> 12u) & 0x3ffu, (api_version & 0xfffu), moon->physical_devices.da.size);
    lake_refcnt_inc(&moon->interface.header.refcnt);
    assembly->out_impl->adapter = moon;
}
#endif /* MOON_VULKAN */
