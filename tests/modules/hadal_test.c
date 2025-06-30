#include "../test_framework.h"

s32 HadalImpl_todo(void *) 
{
    return TEST_RESULT_SKIPPED;
}

static bool interface_impl_is_valid(struct hadal_interface_impl *impl) 
{
    /* if the backend is not supported, just skip */
    if (impl == nullptr) return false;

    /* if any procedure is missing we would like to log this */
    bool is_valid = true;
    lake_drift_push();
    lake_strbuf buf = { .v = lake_drift(2048, 1), .len = 0, .alloc = 4096 };
    char const *whitespace = "\n            ";

#define VALIDATE_PROC(FN) \
    if (impl->FN == nullptr) { is_valid = false; \
        lake_strbuf_appendstrn(&buf, whitespace, 13); \
        lake_strbuf_appendstrn(&buf, "PFN_hadal_" #FN, lake_lengthof("PFN_hadal_" #FN)); \
    }
#ifdef MOON_VULKAN
    VALIDATE_PROC(vulkan_connect_instance)
    VALIDATE_PROC(vulkan_presentation_support)
    VALIDATE_PROC(vulkan_create_surface)
#endif /* MOON_VULKAN */
    VALIDATE_PROC(window_assembly)
    VALIDATE_PROC(window_zero_refcnt)
#undef VALIDATE_PROC
    if (!is_valid) {
        test_log_context();
        test_log("Interface `%s` is not complete, procedures missing: %s", impl->header.name.str, buf.v);
    }
    lake_drift_pop();
    return is_valid;
}

#define IMPL_HADAL_TEST_SUITE(VAR) \
    FN_TEST_SUITE_INIT(HadalImpl_##VAR) \
    { \
        hadal_interface hadal = { .impl = hadal_interface_impl_##VAR(bedrock) }; \
        if (!interface_impl_is_valid(hadal.interface)) { \
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
