if with_platform_windows
    #engine_sources += files()

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

if with_d3d12
    if not cpp.has_header('directx/d3d12.h')
        directx_headers = subproject('directx-headers')
        engine_deps += directx_headers.get_variable('directx_headers_dep')
    endif

    #engine_sources += files()
    pre_args += ['-DMOON_D3D12=1']
endif

if with_asiosdk and cc.check_header('windows.h', required: _audio.contains('asiosdk'))
    #engine_sources += files()
    asiosdk = subproject('asiosdk')
    engine_deps += asiosdk.get_variable('asiosdk_dep')
    pre_args += ['-DSOMA_ASIO=1']
endif

if with_wasapi and cc.check_header('audioclient.h', required: _audio.contains('wasapi'))
    #engine_sources += files()
    pre_args += ['-DSOMA_WASAPI=1']
endif

if with_xaudio2 and dependency('xaudio2', required: _audio.contains('xaudio2'))
    #engine_sources += files()
    pre_args += ['-DSOMA_XAUDIO2=1']
endif
