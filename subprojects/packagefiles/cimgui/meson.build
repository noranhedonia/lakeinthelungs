project(
    'cimgui', 
    ['c', 'cpp'], 
    license: 'MIT', 
    version: '1.91.9b', 
    meson_version: '>=0.50.0',
    default_options: ['cpp_std=c++17'],
)

include_dir = include_directories('.')
sources = files(
    'cimgui.cpp',
    'imgui/imgui.cpp',
    'imgui/imgui_demo.cpp',
    'imgui/imgui_draw.cpp',
    'imgui/imgui_tables.cpp',
    'imgui/imgui_widgets.cpp',
    'imgui/misc/freetype/imgui_freetype.cpp',
)

freetype_dep = dependency('freetype2', fallback: ['freetype', 'freetype_dep'])

cimgui = static_library(
    'cimgui',
    sources,
    cpp_args: [ '-DCIMGUI_NO_EXPORT' ],
    gnu_symbol_visibility: 'hidden',
    dependencies: freetype_dep,
    include_directories: [ include_dir, 'imgui' ],
    install : false,
)

cimgui_pre_args = [ '-DCIMGUI_NO_EXPORT', '-DCIMGUI_DEFINE_ENUMS_AND_STRUCTS' ]
cimgui_dep = declare_dependency(
    include_directories: include_dir,
    link_with: cimgui,
)
