if with_platform_android
    #engine_sources += files('android.c')
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

if with_aaudio and cc.find_library('aaudio', required: with_platform_android).found()
    #engine_sources += files()
    pre_args += ['-DSOMA_AAUDIO=1']
endif
