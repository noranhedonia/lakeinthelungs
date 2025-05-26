project(
    'cimgui', 
    ['c', 'cpp'], 
    license: 'MIT', 
    version: '1.91.9b', 
    meson_version: '>=0.50.0',
    default_options: ['cpp_std=c++11'],
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

api = '-DIMGUI_API=__attribute__((visibility("default")))'
if host_machine.system() == 'windows'
    api = '-DIMGUI_API=@0@'.format(
        get_option('default_library') != 'static' ? '__declspec(dllexport)' : '',
    )
endif

freetype_dep = dependency('freetype2', fallback: ['freetype', 'freetype_dep'])

cimgui = library(
    'cimgui',
    sources,
    cpp_args: api,
    gnu_symbol_visibility: 'hidden',
    dependencies: freetype_dep,
    include_directories: [ include_dir, 'imgui' ],
    install : true,
)

if host_machine.system() == 'windows'
    api = '-DIMGUI_API=@0@'.format(
        get_option('default_library') != 'static' ? '__declspec(dllimport)' : '',
    )
endif

cimgui_dep = declare_dependency(
    compile_args: api,
    include_directories: include_dir,
    link_with: cimgui,
)
