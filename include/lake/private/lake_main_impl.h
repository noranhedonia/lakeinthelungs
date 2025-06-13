#if !defined(LAKE_IN_THE_LUNGS_MAIN) || defined(__LAKE_IN_THE_LUNGS_MAIN_IMPL__) 
#error "This header should only be included once, with LAKE_IN_THE_LUNGS_MAIN defined."
#endif

#include <lake/bedrock.h>

#undef LAKE_IN_THE_LUNGS_MAIN
#define __LAKE_IN_THE_LUNGS_MAIN_IMPL__

/** This function must be implemented by the application in the 
 *  main source file, where LAKE_IN_THE_LUNGS_MAIN was defined. */
extern s32 LAKECALL lake_main(lake_framework *framework, s32 argc, char const *argv[]);

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
    lake_framework framework = {
        .engine_name = "A Moonlit Walk",
        .build_engine_ver = LAKE_VERSION_NUM(0, 1, 0),
        .hints.frames_in_flight = 3,
#ifdef LAKE_DEBUG
        .hints.enable_debug_instruments = true,
#endif /* LAKE_DEBUG */
    };
    return lake_main(&framework, argc, argv);
}
#endif
