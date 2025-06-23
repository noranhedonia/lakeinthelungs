project('lakeinthelungs', ['c'],
    version: files('VERSION'),
    license: 'MIT',
    meson_version: '>= 1.5.0',
    default_options: [
        'buildtype=debugoptimized',
        'b_ndebug=if-release',
        'default_library=shared',
        'warning_level=3',
        'c_std=c23',
        'cimgui:default_library=static',
    ],
)

if host_machine.system() == 'darwin'
    add_languages('objc', native: false)
    add_project_arguments('-fobjc-arc', language: 'objc')
    libname_suffix = 'dylib'
elif host_machine.system() == 'windows'
    libname_suffix = 'dll'
else
    libname_suffix = 'so'
endif

cc = meson.get_compiler('c')
#cpp = meson.get_compiler('cpp')
fs = import('fs')

if get_option('layout') != 'mirror'
    error('`mirror` is the only build directory layout supported.')
endif

with_lake_debug = get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized'
# this means the final value of b_ndebug is true
with_lake_ndebug = get_option('b_ndebug') == 'true' or (get_option('buildtype') == 'release' and get_option('b_ndebug') == 'if-release')

relative_dir = fs.relative_to(meson.global_source_root(), meson.global_build_root())

# arguments for the preprocessor, they need to be added to the default arguments for both C and C++
pre_args = [
    '-Werror=all',
    '-D__STDC_CONSTANT_MACROS',
    '-D__STDC_FORMAT_MACROS',
    '-D__STDC_LIMIT_MACROS',
    '-DLAKE_BUILD_VERSION="@0@"'.format(meson.project_version()),
    '-DLAKE_BUILD_BUGREPORT="https://github.com/cadencecmoon/lakeinthelungs/issues"',
]
if with_lake_ndebug
    pre_args += ['-DNDEBUG', '-DLAKE_NDEBUG']
elif with_lake_debug
    pre_args += ['-DDEBUG', '-DLAKE_DEBUG']
endif

# arguments for C or C++ compilers
c_args = ['-Wno-pedantic']
#cpp_args = ['-Wno-pedantic']
include_dirs = [ include_directories('include') ]

subdir('source')
if get_option('build-main')
main = executable(
    'main', [ 'main.c' ],
    dependencies: [ amwengine_dep ],
    install: true,
)
endif
if get_option('build-tests')
    subdir('tests')
endif

doxygen = find_program('doxygen', required : get_option('build-docs'))
if doxygen.found()
    docs_cdata = configuration_data()
    docs_cdata.set('VERSION', meson.project_version())
    docs_cdata.set('TOP_SRCDIR', meson.project_source_root())
    docs_cdata.set('TOP_BUILDDIR', meson.project_source_root())

    doxyfile = configure_file(
        input : 'Doxyfile.in',
        output : 'Doxyfile',
        configuration : docs_cdata,
        install : false)
    
    docs_datadir = get_option('build-docs-path')
    if docs_datadir.contains('')
        docs_datadir = '$datadir/docs/lake'
    endif

    html_target = custom_target(
        'lakeinthelungs',
        input : doxyfile,
        output : 'docs/html',
        command : [ doxygen, doxyfile ],
        install_dir : docs_datadir,
        install : true)
endif

# TODO summary
