project('vulkan-headers', [ 'c', 'cpp'], license: 'Apache-2.0', version: '1.4.309')

vulkan_api_xml = files(join_paths('registry', 'vk.xml'))

vulkan_headers_dep = declare_dependency(
    include_directories: include_directories('include'),
)
