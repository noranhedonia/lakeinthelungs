engine_sources = files(
    'bedrock.c',
    'frame_time.c',
    'log.c',
    'malloc.c',
    'moon.c',
    'work.c',
)

threads_dep = dependency('threads', required: false)
math_dep = cc.find_library('m', required: false)

engine_deps = [ threads_dep, math_dep ]
engine_link_args = []

system_has_kms_drm = ['openbsd', 'netbsd', 'freebsd', 'gnu/kfreebsd', 'dragonfly', 'linux', 'sunos', 'android', 'managarm'].contains(host_machine.system())

_platforms = get_option('platforms')
with_platform_xcb = _platforms.contains('xcb')
with_platform_wayland = _platforms.contains('wayland')
with_platform_kms_drm = _platforms.contains('kms_drm')
with_platform_android = _platforms.contains('android')
with_platform_haiku = _platforms.contains('haiku')
with_platform_emscripten = _platforms.contains('emscripten')
with_platform_windows = _platforms.contains('windows')
with_platform_macos = _platforms.contains('cocoa')
with_platform_ios = _platforms.contains('uikit')
with_platform_posix = (not with_platform_windows) and cc.has_header_symbol('unistd.h', '_POSIX_VERSION')

if _platforms.contains('auto')
    if host_machine.system() == 'android'
        _platforms = ['android']
        with_platform_android = true
        with_platform_kms_drm = true
    elif system_has_kms_drm
        _platforms = ['kms_drm']
        with_platform_kms_drm = true
        with_platform_wayland = true
        with_platform_xcb = true
    elif host_machine.system() == 'cygwin'
        _platforms = ['xcb','windows']
        with_platform_xcb = true
    elif host_machine.system() == 'haiku'
        _platforms = ['haiku']
        with_platform_haiku = true
    elif host_machine.system() == 'emscripten'
        _platforms = ['emscripten']
        with_platform_emscripten = true
    elif host_machine.system() == 'windows'
        _platforms = ['windows']
        with_platform_windows = true
    elif host_machine.system() == 'darwin'
        iphone_check = 
         '''#include <TargetConditionals.h>
            #if !TARGET_OS_IPHONE
            #error not iphone
            #endif'''
        if cc.compiles(iphone_check, name: 'iPhone target')
            _platforms = ['uikit']
            with_platform_ios = true
        else
            _platforms = ['cocoa']
            with_platform_macos = true
            with_platform_xcb = true
        endif
    else
        error('Unknown OS @0@. Please pass -Dplatforms to set the target platforms to build support for.'.format(host_machine.system()))
    endif
endif

_audio = get_option('audio')
with_pipewire = _audio.contains('pipewire')
with_pulseaudio = _audio.contains('pulseaudio')
with_jack = _audio.contains('jack')
with_alsa = _audio.contains('alsa')
with_aaudio = _audio.contains('aaudio')
with_asiosdk = _audio.contains('asiosdk')
with_wasapi = _audio.contains('wasapi')
with_xaudio2 = _audio.contains('xaudio2')
with_coreaudio = _audio.contains('coreaudio')
with_webaudio = _audio.contains('webaudio')

if _audio.contains('auto')
    with_pipewire = with_pipewire or system_has_kms_drm
    with_pulseaudio = with_pulseaudio or system_has_kms_drm
    with_jack = with_jack or with_platform_posix or with_platform_windows
    with_alsa = with_alsa or system_has_kms_drm
    with_aaudio = with_aaudio or with_platform_android
    with_asiosdk = with_asiosdk or with_platform_windows
    with_wasapi = with_wasapi or with_platform_windows
    with_xaudio2 = with_xaudio2 or with_platform_windows
    with_coreaudio = with_coreaudio or with_platform_macos or with_platform_ios
    with_webaudio = with_webaudio or with_platform_emscripten
endif

_graphics = get_option('graphics')
with_vulkan = _graphics.contains('vulkan')
with_d3d12 = _graphics.contains('d3d12')
with_metal = _graphics.contains('metal')
with_webgpu = _graphics.contains('webgpu')

if _graphics.contains('auto')
    with_vulkan = with_vulkan or not with_platform_emscripten
    with_d3d12 = with_d3d12 or with_platform_windows
    with_metal = with_metal or with_platform_macos or with_platform_ios
    with_webgpu = with_webgpu or with_platform_emscripten
endif

if not cpp.has_header('slang.h')
    slang = subproject('slang')
    engine_deps += slang.get_variable('slang_include_dep')
endif
pre_args += ['-DLAKE_SLANG=1']

with_imgui = get_option('imgui')
if with_imgui
    cimgui = subproject('cimgui')
    cimgui_dep = cimgui.get_variable('cimgui_dep')
    engine_deps += cimgui_dep
    pre_args += ['-DLAKE_IMGUI=1']
endif

ogg_dep = dependency('ogg', fallback : ['ogg', 'ogg_dep'])
vorbis_dep = dependency('vorbis', fallback : ['vorbis', 'vorbis_dep'])
if ogg_dep.found() and vorbis_dep.found()
    # XXX vorbis linked at runtime ?
    engine_deps += [ ogg_dep, vorbis_dep ]
    pre_args += '-DLAKE_VORBIS=1'
endif

subdir('android')
subdir('apple')
subdir('asm')
subdir('data_structures')
subdir('emscripten')
subdir('linux')
subdir('math')
subdir('unix')
subdir('vulkan')
subdir('windows')

add_project_arguments(pre_args, language: ['c', 'cpp'])
add_project_arguments(c_args, language: ['c'])
add_project_arguments(cpp_args, language: ['cpp'])

amwengine = library(
    'amwengine', engine_sources,
    dependencies: engine_deps,
    include_directories: include_dir,
    install: true)

amwengine_dep = declare_dependency(
    link_with: amwengine,
    link_args: engine_link_args,
    include_directories: include_dir)
