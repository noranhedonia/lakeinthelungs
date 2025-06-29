#include <lake/bedrock/bedrock.h>

#if defined(LAKE_PLATFORM_UNIX)
#include <dlfcn.h>

void *lake_open_library(char const *libname)
{
    void *library = dlopen(libname, RTLD_NOW | RTLD_LOCAL);
#ifndef LAKE_NDEBUG
    if (!library) 
        lake_error("dlopen '%s' failed: %s.", libname, dlerror());
#endif
    return library;
}

void lake_close_library(void *library)
{
    dlclose(library);
}

void *lake_get_proc_address(void *library, char const *procname)
{
#ifndef LAKE_NDEBUG
    char const *err;
    void *addr = dlsym(library, procname);
    if ((err = dlerror()) != nullptr)
        lake_error("dlsym '%s' failed: %s.", procname, err);
    return addr;
#else
    return dlsym(library, procname);
#endif
}
#endif /* LAKE_PLATFORM_UNIX */
