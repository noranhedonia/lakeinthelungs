engine_sources += files(
    'bedrock.c',
    'data_structures.c',
    'drifter.c',
    'frame_time.c',
    'job_system.c',
    'log.c',
    'machina.c',
    'network.c',
    'tagged_heap.c',
    'temp_backend_stubs.c',
)

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
    if cc.has_header('execinfo.h')
        pre_args += ['-DLAKE_HAS_EXECINFO=1']
    endif
    pre_args += ['-DLAKE_PLATFORM_LINUX=1']
endif
