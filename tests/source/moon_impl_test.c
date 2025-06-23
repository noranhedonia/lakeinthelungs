#include "../framework.h"

s32 MoonImpl_todo(void *) 
{
    return TEST_RESULT_SKIPPED;
}

#define IMPL_MOON_TEST_SUITE(VAR) \
    FN_TEST_SUITE_INIT(MoonImpl_##VAR) \
    { \
        moon_interface moon = { .impl = moon_interface_impl_##VAR(framework) }; \
        if (moon.impl == nullptr) { \
            *out = (struct test_suite_details){0}; \
            return; \
        } \
        *out = (struct test_suite_details){ \
            .count = lake_arraysize(g_tests), \
            .tests = g_tests, \
            .userdata = moon.impl, \
        }; \
    } \
    FN_TEST_SUITE_FINI(MoonImpl_##VAR) \
    { \
        moon_interface moon = { .v = userdata }; \
        moon_interface_unref(moon); \
    }

static struct test_case_details g_tests[] = {
    IMPL_TEST_CASE(MoonImpl, todo),
};

#ifdef MOON_D3D12
IMPL_MOON_TEST_SUITE(d3d12)
#endif /* MOON_D3D12 */
#ifdef MOON_METAL
IMPL_MOON_TEST_SUITE(metal)
#endif /* MOON_METAL */
#ifdef MOON_WEBGPU
IMPL_MOON_TEST_SUITE(webgpu)
#endif /* MOON_WEBGPU */
#ifdef MOON_VULKAN
IMPL_MOON_TEST_SUITE(vulkan)
#endif /* MOON_VULKAN */
IMPL_MOON_TEST_SUITE(mock)
