#include "../framework.h"

s32 HadeanImpl_todo(void *) 
{
    return TEST_RESULT_SKIPPED;
}

static bool interface_impl_is_valid(struct hadean_interface_impl *impl) 
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
        lake_strbuf_appendstrn(&buf, "PFN_hadean_" #FN, lake_lengthof("PFN_hadean_" #FN)); \
    }
    (void)whitespace;
    /* TODO */
#undef VALIDATE_PROC
    if (!is_valid) {
        test_log_context();
        test_log("Interface `%s` is not complete, procedures missing: %s", impl->header.name.str, buf.v);
    }
    lake_drift_pop();
    return is_valid;
}

#define IMPL_HADEAN_TEST_SUITE(VAR) \
    FN_TEST_SUITE_INIT(HadeanImpl_##VAR) \
    { \
        hadean_interface hadean = { .impl = hadean_interface_impl_##VAR(bedrock) }; \
        if (!interface_impl_is_valid(hadean.interface)) { \
            *out = (struct test_suite_details){0}; \
            return; \
        } \
        *out = (struct test_suite_details){ \
            .count = lake_arraysize(g_tests), \
            .tests = g_tests, \
            .userdata = hadean.impl, \
        }; \
    } \
    FN_TEST_SUITE_FINI(HadeanImpl_##VAR) \
    { \
        hadean_interface hadean = { .v = userdata }; \
        hadean_interface_unref(hadean); \
    }

static struct test_case_details g_tests[] = {
    IMPL_TEST_CASE(HadeanImpl, todo),
};

#ifdef HADEAN_OPENXR
IMPL_HADEAN_TEST_SUITE(openxr)
#endif /* HADEAN_OPENXR */
#ifdef HADEAN_WEBXR
IMPL_HADEAN_TEST_SUITE(webxr)
#endif /* HADEAN_WEBXR */
IMPL_HADEAN_TEST_SUITE(headless)
