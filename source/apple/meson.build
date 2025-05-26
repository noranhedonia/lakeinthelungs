if with_platform_macos or with_platform_ios
    pre_args += ['-DLAKE_PLATFORM_APPLE=1', '-DVK_USE_PLATFORM_METAL_EXT'] 

    if with_platform_macos
        #engine_sources += files('cocoa.m')
        pre_args += ['-DLAKE_PLATFORM_MACOS=1', '-DHADAL_COCOA=1']
    endif

    if with_platform_ios
        #engine_sources += files('uikit.m')
        engine_deps += declare_dependency(link_args: ['-framework', 'UIKIT'])
        pre_args += ['-DLAKE_PLATFORM_IOS=1', '-DHADAL_UIKIT=1']
    endif
endif

if with_metal
    metal_dep = dependency('appleframeworks', modules: ['Metal'], required: _rendering.contains('metal'))
    if metal_dep.found()
        #engine_sources += files()
        engine_deps += metal_dep
        pre_args += ['-DMOON_METAL=1']
    endif
endif

if with_coreaudio
    core_dep = dependency('appleframeworks', modules: ['CoreAudio', 'CoreFoundation'], required: _audio.contains('coreaudio'))
    if core_dep.found()
        #engine_sources += files()
        engine_deps += core_dep
        pre_args += ['-DSOMA_COREAUDIO=1']
    endif
endif
