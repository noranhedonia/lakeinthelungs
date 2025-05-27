#if !defined(LAKE_IN_THE_LUNGS_MAIN) || defined(__LAKE_IN_THE_LUNGS_MAIN_IMPL__) 
#error "This header should only be included once, with LAKE_IN_THE_LUNGS_MAIN defined."
#endif

#include <lake/bedrock/types.h>

#undef LAKE_IN_THE_LUNGS_MAIN
#define __LAKE_IN_THE_LUNGS_MAIN_IMPL__

/** This function must be implemented by the application in the 
 *  main source file, where LAKE_IN_THE_LUNGS_MAIN was defined. */
extern s32 LAKECALL lake_main(s32 argc, char const *argv[]);

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
    return lake_main(argc, argv);
}
#endif
