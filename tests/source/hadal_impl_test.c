#include "../framework.h"

s32 HadalImpl_todo(void *) 
{
    return TEST_RESULT_SKIPPED;
}

#define IMPL_HADAL_TEST_SUITE(VAR) \
    FN_TEST_SUITE_INIT(HadalImpl_##VAR) \
    { \
        hadal_interface hadal = { .impl = hadal_interface_impl_##VAR(framework) }; \
        if (hadal.impl == nullptr) { \
            *out = (struct test_suite_details){0}; \
            return; \
        } \
        *out = (struct test_suite_details){ \
            .count = lake_arraysize(g_tests), \
            .tests = g_tests, \
            .userdata = hadal.impl, \
        }; \
    } \
    FN_TEST_SUITE_FINI(HadalImpl_##VAR) \
    { \
        hadal_interface hadal = { .v = userdata }; \
        hadal_interface_unref(hadal); \
    }

static struct test_case_details g_tests[] = {
    IMPL_TEST_CASE(HadalImpl, todo),
};

#ifdef HADAL_WIN32
IMPL_HADAL_TEST_SUITE(win32)
#endif /* HADAL_WIN32 */
#ifdef HADAL_COCOA
IMPL_HADAL_TEST_SUITE(cocoa)
#endif /* HADAL_COCOA */
#ifdef HADAL_UIKIT
IMPL_HADAL_TEST_SUITE(uikit)
#endif /* HADAL_UIKIT */
#ifdef HADAL_ANDROID
IMPL_HADAL_TEST_SUITE(android)
#endif /* HADAL_ANDROID */
#ifdef HADAL_HAIKU
IMPL_HADAL_TEST_SUITE(haiku)
#endif /* HADAL_HAIKU */
#ifdef HADAL_HTML5
IMPL_HADAL_TEST_SUITE(html5)
#endif /* HADAL_HTML5 */
#ifdef HADAL_WAYLAND
IMPL_HADAL_TEST_SUITE(wayland)
#endif /* HADAL_WAYLAND */
#ifdef HADAL_XCB
IMPL_HADAL_TEST_SUITE(xcb)
#endif /* HADAL_XCB */
#ifdef HADAL_KMS
IMPL_HADAL_TEST_SUITE(kms)
#endif /* HADAL_KMS */
IMPL_HADAL_TEST_SUITE(headless)
