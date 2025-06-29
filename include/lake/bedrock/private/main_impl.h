#if !defined(LAKE_IN_THE_LUNGS_MAIN) || defined(__LAKE_IN_THE_LUNGS_MAIN_IMPL__) 
#error "This header should only be included once, in the main source file with LAKE_IN_THE_LUNGS_MAIN defined."
#endif

#include <lake/bedrock/bedrock.h>

#undef LAKE_IN_THE_LUNGS_MAIN
#define __LAKE_IN_THE_LUNGS_MAIN_IMPL__

/** This function must be implemented by the application in the 
 *  main source file, where LAKE_IN_THE_LUNGS_MAIN was defined. */
extern s32 LAKECALL lake_main(lake_bedrock *bedrock, s32 argc, char const *argv[]);

#if defined(LAKE_PLATFORM_WINDOWS)
#include <lake/bedrock/private/windows.h>
/* TODO */
#elif defined(LAKE_PLATFORM_ANDROID)
/* TODO */
#elif defined(LAKE_PLATFORM_EMSCRIPTEN)
/* TODO */
#else
s32 main(s32 argc, char const *argv[])
{
    lake_bedrock bedrock = {
        .engine_name = "Lake in the Lungs",
        .app_name = bedrock.engine_name,
        .build_engine_ver = LAKE_VERSION,
        .build_app_ver = LAKE_VERSION,
        .hints.frames_in_flight = 2,
#ifdef LAKE_DEBUG
        .hints.enable_debug_instruments = true,
#endif /* LAKE_DEBUG */
        .hints.networking_offline = false,
    };
    return lake_main(&bedrock, argc, argv);
}
#endif
