if with_platform_kms_drm
    engine_sources += files(
        'linux_proc.c',
    )
    udev_dep = dependency('libudev', required: _platforms.contains('kms_drm'))
    drm_dep = dependency('libdrm', required: _platforms.contains('kms_drm'))
    gbm_dep = dependency('gbm', required: _platforms.contains('kms_drm'))

    if cc.has_header('execinfo.h')
        pre_args += ['-DLAKE_HAS_EXECINFO=1']
    endif

    if udev_dep.found() and drm_dep.found() and gbm_dep.found()
        engine_sources += files('kms.c')
        pre_args += ['-DLAKE_PLATFORM_LINUX=1', '-DHADAL_KMS=1', '-DVK_USE_PLATFORM_KMS_KHR']
    endif
endif

if with_platform_xcb
    xcb_dep = dependency('xcb', required: _platforms.contains('xcb'))
    xcb_icccm_dep = dependency('xcb-icccm', required: _platforms.contains('xcb'))

    if xcb_dep.found() and xcb_icccm_dep.found()
        engine_sources += files('xcb.c')
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
            'wl_display.c',
            'wl_vulkan.c',
            'wl_window.c',
        )
        pre_args += ['-DHADAL_WAYLAND=1', '-DVK_USE_PLATFORM_WAYLAND_KHR']

        wayland_protocols_dir = '../../wayland-protocols/'
        client_header_args = [ wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@' ]
        private_code_args = [ wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@' ]

        if cc.has_header('libdecor.h')
            pre_args += '-DHADAL_LIBDECOR=1'
        endif

        wayland_protocols = [
            'viewporter',
            'wayland',
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

if with_pipewire and dependency('libpipewire-0.3', required: _audio.contains('pipewire')).found()
    engine_sources += files('pw_soma.c')
    pre_args += ['-DSOMA_PIPEWIRE=1']
endif

if with_pulseaudio and dependency('libpulse-simple', required: _audio.contains('pulseaudio')).found()
    engine_sources += files('pulseaudio.c')
    pre_args += ['-DSOMA_PULSEAUDIO=1']
endif

if with_alsa and dependency('alsa', required: _audio.contains('alsa')).found()
    engine_sources += files('alsa.c')
    pre_args += ['-DSOMA_ALSA=1']
endif
