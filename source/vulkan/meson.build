if with_vulkan
    engine_sources += files(
        'vk_command_recorder.c',
        'vk_device.c',
        'vk_drivers.c',
        'vk_gpu_resources.c',
        'vk_pipelines.c',
        'vk_swapchain.c',
        'vk_timeline.c',
    )
    pre_args += ['-DMOON_VULKAN=1']

    if not cc.has_header('vulkan/vulkan.h')
        vulkan_headers = subproject('vulkan-headers')
        engine_deps += vulkan_headers.get_variable('vulkan_headers_dep')
    endif

    vulkan_memory_allocator = subproject('vulkan-memory-allocator')
    engine_deps += vulkan_memory_allocator.get_variable('vma_dep')

    spirv_headers_dep = dependency('SPIRV-Headers', fallback: ['spirv-headers', 'spirv_headers_dep'], required: false)
    spirv_tools_dep = dependency('SPIRV-Tools', fallback: ['spirv-tools', 'spirv_tools_dep'], required: false)
    if spirv_headers_dep.found() and spirv_tools_dep.found()
        engine_deps += [spirv_headers_dep, spirv_tools_dep]
        pre_args += ['-DMOON_SPIRV=1']
    endif
endif
