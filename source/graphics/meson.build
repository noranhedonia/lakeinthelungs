if not cc.has_header('slang.h') or get_option('slang-runtime')
    slang = subproject('slang')
    if slang.get_option('default_library') == 'static'
        engine_deps += slang.get_variable('slang_dep')
        pre_args += '-DLAKE_SLANG_STATIC=1'
    else
        engine_deps += slang.get_variable('slang_headers_dep')
    endif
endif
pre_args += ['-DLAKE_SLANG=1']

with_renderdoc = false
if get_option('renderdoc').auto()
    with_renderdoc = with_lake_debug
else
    with_renderdoc = get_option('renderdoc').enabled()
endif
if with_renderdoc and cc.has_header('renderdoc_app.h')
    pre_args += '-DLAKE_HAS_RENDERDOC=1'
endif

if with_d3d12
    if not cpp.has_header('directx/d3d12.h')
        directx_headers = subproject('directx-headers')
        engine_deps += directx_headers.get_variable('directx_headers_dep')
    endif

    #engine_sources += files(TODO)
    pre_args += ['-DMOON_D3D12=1']
endif

if with_metal
    metal_dep = dependency('appleframeworks', modules: ['Metal'], required: _rendering.contains('metal'))
    if metal_dep.found()
        #engine_sources += files(TODO)
        engine_deps += metal_dep
        pre_args += ['-DMOON_METAL=1']
    endif
endif

if with_vulkan
    engine_sources += files(
        'vk_command_recorder.c',
        'vk_device.c',
        'vk_pipeline.c',
        'vk_gpu_resources.c',
        'vk_moon.c',
        'vk_swapchain.c',
        'vk_timeline_sync.c',
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
        pre_args += ['-DMOON_VULKAN_SPIRV=1']
    endif
endif
