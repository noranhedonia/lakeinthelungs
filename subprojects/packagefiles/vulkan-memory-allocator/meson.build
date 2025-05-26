project('vulkan-memory-allocator', ['c', 'cpp'], version : '3.2.1')

include_dir = include_directories('include')

vma = library(
    'vma',
    'vk_mem_alloc.cpp',
    include_directories: include_dir,
    install: true
)

vma_dep = declare_dependency(
    link_with: vma,
    include_directories: include_dir
)
