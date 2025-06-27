if with_asiosdk and cc.check_header('windows.h', required: _audio.contains('asiosdk'))
    #engine_sources += files(TODO)
    asiosdk = subproject('asiosdk')
    engine_deps += asiosdk.get_variable('asiosdk_dep')
    pre_args += ['-DSOMA_ASIO=1']
endif

if with_wasapi and cc.check_header('audioclient.h', required: _audio.contains('wasapi'))
    #engine_sources += files(TODO)
    pre_args += ['-DSOMA_WASAPI=1']
endif

if with_xaudio2 and dependency('xaudio2', required: _audio.contains('xaudio2'))
    #engine_sources += files(TODO)
    pre_args += ['-DSOMA_XAUDIO2=1']
endif

if with_coreaudio
    core_dep = dependency('appleframeworks', modules: ['CoreAudio', 'CoreFoundation'], required: _audio.contains('coreaudio'))
    if core_dep.found()
        #engine_sources += files(TODO)
        engine_deps += core_dep
        pre_args += ['-DSOMA_COREAUDIO=1']
    endif
endif

if with_aaudio and cc.find_library('aaudio', required: with_platform_android).found()
    #engine_sources += files(TODO)
    pre_args += ['-DSOMA_AAUDIO=1']
endif

if with_pipewire and dependency('libpipewire-0.3', required: _audio.contains('pipewire')).found()
    engine_sources += files('pw_soma.c')
    pre_args += ['-DSOMA_PIPEWIRE=1']
endif

if with_pulseaudio and dependency('libpulse-simple', required: _audio.contains('pulseaudio')).found()
    #engine_sources += files(TODO)
    pre_args += ['-DSOMA_PULSEAUDIO=1']
endif

if with_jack and dependency('jack', required: _audio.contains('jack')).found()
    #engine_sources += files(TODO)
    pre_args += ['-DSOMA_JACK=1']
endif

if with_alsa and dependency('alsa', required: _audio.contains('alsa')).found()
    #engine_sources += files(TODO)
    pre_args += ['-DSOMA_ALSA=1']
endif
