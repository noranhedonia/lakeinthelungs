if with_platform_posix
    engine_sources += files(
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

if with_jack and dependency('jack', required: _audio.contains('jack')).found()
    #engine_sources += files()
    pre_args += ['-DSOMA_JACK=1']
endif
