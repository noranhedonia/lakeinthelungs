if with_platform_android
    #engine_sources += files(TODO)
    engine_link_args += ['-u', 'ANativeActivity_onCreate']
    pre_args += ['-DLAKE_PLATFORM_ANDROID=1', '-DHADAL_ANDROID=1', '-DVK_USE_PLATFORM_ANDROID_KHR']

    android_ndk = get_option('android-ndk-dir')
    if android_ndk.contains('')
        # provide default path, valid for unix
        android_ndk = '/opt/android-ndk'
    endif

    android_dep = cc.find_library('android', required: true)
    log_dep = cc.find_library('log', required: true)

    android_native_glue = static_library('android_app_glue',
        include_directories : [include_directories(android_ndk + '/sources/android/native_app_glue')],
        [android_ndk + '/sources/android/native_app_glue/android_native_app_glue.c'])
    android_native_glue_dep = declare_dependency(
        include_directories : [include_directories(android_ndk + '/sources/android/native_app_glue')],
        link_with : android_native_glue)
    engine_deps += [android_dep, log_dep, android_native_glue_dep]
endif

if with_platform_macos or with_platform_ios
    pre_args += ['-DLAKE_PLATFORM_APPLE=1', '-DVK_USE_PLATFORM_METAL_EXT'] 

    if with_platform_macos
        #engine_sources += files(TODO)
        pre_args += ['-DLAKE_PLATFORM_MACOS=1', '-DHADAL_COCOA=1']
    endif

    if with_platform_ios
        #engine_sources += files(TODO)
        engine_deps += declare_dependency(link_args: ['-framework', 'UIKIT'])
        pre_args += ['-DLAKE_PLATFORM_IOS=1', '-DHADAL_UIKIT=1']
    endif
endif

if with_platform_posix
    engine_sources += files(
        'posix_dlfcn.c',
        'posix_mmap.c',
        'posix_threads.c',
        'posix_time.c',
    )
    dl_dep = cc.find_library('dl', required: false)
    rt_dep = cc.find_library('rt', required: false)

    if cc.has_function('clock_gettime', dependencies: [rt_dep])
        pre_args += ['-DLAKE_HAS_CLOCK_GETTIME=1']
    endif

    engine_deps += [dl_dep, rt_dep]
    pre_args += ['-DLAKE_PLATFORM_UNIX=1', '-D_GNU_SOURCE']
endif

if with_platform_windows
    #engine_sources += files(TODO)
    pre_args += [
        '-DLAKE_PLATFORM_WINDOWS=1', 
        '-DHADAL_WIN32=1',
        '-DVK_USE_PLATFORM_WIN32_KHR',
        '-DUNICODE=1', 
        '-D_UNICODE=1',
        '-D_WIN32_WINNT=0x0A00', 
        '-DVC_EXTRALEAN',
        '-DWIN32_LEAN_AND_MEAN',
        '-DNOGDI', 
        '-DNOMINMAX']
endif

if with_platform_kms
    engine_sources += files(
        'linux_filesystem.c',
        'linux_proc.c',
    )
    udev_dep = dependency('libudev', required: _platforms.contains('kms'))
    drm_dep = dependency('libdrm', required: _platforms.contains('kms'))
    gbm_dep = dependency('gbm', required: _platforms.contains('kms'))

    if cc.has_header('execinfo.h')
        pre_args += ['-DLAKE_HAS_EXECINFO=1']
    endif

    if udev_dep.found() and drm_dep.found() and gbm_dep.found()
        #engine_sources += files(TODO)
        pre_args += ['-DLAKE_PLATFORM_LINUX=1', '-DHADAL_KMS=1', '-DVK_USE_PLATFORM_KMS_KHR']
    endif
endif

if with_platform_xcb
    xcb_dep = dependency('xcb', required: _platforms.contains('xcb'))
    xcb_icccm_dep = dependency('xcb-icccm', required: _platforms.contains('xcb'))

    if xcb_dep.found() and xcb_icccm_dep.found()
        #engine_sources += files(TODO)
        pre_args += ['-DHADAL_XCB=1', '-DVK_USE_PLATFORM_XCB_KHR']
    endif
endif

if with_platform_wayland
    wayland_scanner = find_program('wayland-scanner', required: _platforms.contains('wayland'))
    wayland_client_dep = dependency('wayland-client', required: _platforms.contains('wayland'))
    wayland_cursor_dep = dependency('wayland-cursor', required: _platforms.contains('wayland'))
    xkbcommon_dep = dependency('xkbcommon', required: _platforms.contains('wayland'))

    if wayland_scanner.found() and wayland_client_dep.found() and wayland_cursor_dep.found() and xkbcommon_dep.found()
        engine_sources += files(
            'wl_hadal.c',
        )
        pre_args += ['-DHADAL_WAYLAND=1', '-DVK_USE_PLATFORM_WAYLAND_KHR']

        wayland_protocols_dir = '../../wayland-protocols/'
        client_header_args = [ wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@' ]
        private_code_args = [ wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@' ]

        if cc.has_header('libdecor.h')
            pre_args += '-DHADAL_LIBDECOR=1'
        endif

        wayland_protocols = [
            'fractional-scale-v1',
            'idle-inhibit-unstable-v1',
            'tablet-unstable-v2',
            'viewporter',
            'wayland',
            'xdg-activation-v1',
            'xdg-shell',
        ]

        foreach name : wayland_protocols
            protocol_xml_path = wayland_protocols_dir + '@0@.xml'.format(name)

            client_header = custom_target(
                ['@0@ client-header'.format(name)],
                command : client_header_args,
                input : protocol_xml_path,
                output : name + '-protocol.h',
                install : false)

            private_code = custom_target(
                ['@0@ private-code'.format(name)],
                command : private_code_args,
                input : protocol_xml_path,
                output : name + '-protocol-code.h',
                install : false)

            engine_sources += [ client_header, private_code ]
            message('Generating Wayland \'' + name + '.xml\' protocol code.')
        endforeach
    endif
endif

_xr = get_option('xr-runtimes')
with_openxr = _platforms.contains('openxr')
with_webxr = _platforms.contains('webxr')

if _xr.contains('auto')
    with_webxr = with_platform_emscripten
    with_openxr = not with_platform_emscripten
endif

# TODO
if with_openxr
    pre_args += '-DHADEAN_OPENXR=1'
elif with_webxr
    pre_args += '-DHADEAN_WEBXR=1'
endif
