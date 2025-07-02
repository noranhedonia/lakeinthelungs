#include "../test_framework.h"

s32 SomaImpl_todo(void *) 
{
    return TEST_RESULT_SKIPPED;
}

static bool interface_impl_is_valid(struct soma_interface_impl *impl) 
{
    /* if the backend is not supported, just skip */
    if (impl == nullptr) return false;

    /* if any procedure is missing we would like to log this */
    bool is_valid = true;
    lake_drift_push();
    lake_strbuf buf = { .v = lake_drift_allocate(2048, 1), .len = 0, .alloc = 2048 };
    char const *whitespace = "\n            ";

#define VALIDATE_PROC(FN) \
    if (impl->FN == nullptr) { is_valid = false; \
        lake_strbuf_appendstrn(&buf, whitespace, 13); \
        lake_strbuf_appendstrn(&buf, "PFN_soma_" #FN, lake_lengthof("PFN_soma_" #FN)); \
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

#define IMPL_SOMA_TEST_SUITE(VAR) \
    FN_TEST_SUITE_INIT(SomaImpl_##VAR) \
    { \
        soma_interface soma = { .impl = soma_interface_impl_##VAR(bedrock) }; \
        if (!interface_impl_is_valid(soma.interface)) { \
            *out = (struct test_suite_details){0}; \
            return; \
        } \
        *out = (struct test_suite_details){ \
            .count = lake_arraysize(g_tests), \
            .tests = g_tests, \
            .userdata = soma.impl, \
        }; \
    } \
    FN_TEST_SUITE_FINI(SomaImpl_##VAR) \
    { \
        soma_interface soma = { .v = userdata }; \
        soma_interface_unref(soma); \
    }

static struct test_case_details g_tests[] = {
    IMPL_TEST_CASE(SomaImpl, todo),
};

#ifdef SOMA_ASIO
IMPL_SOMA_TEST_SUITE(asio)
#endif /* SOMA_ASIO */
#ifdef SOMA_WASAPI
IMPL_SOMA_TEST_SUITE(wasapi)
#endif /* SOMA_WASAPI */
#ifdef SOMA_XAUDIO2
IMPL_SOMA_TEST_SUITE(xaudio2)
#endif /* SOMA_XAUDIO2 */
#ifdef SOMA_COREAUDIO
IMPL_SOMA_TEST_SUITE(coreaudio)
#endif /* SOMA_COREAUDIO */
#ifdef SOMA_AAUDIO
IMPL_SOMA_TEST_SUITE(aaudio)
#endif /* SOMA_AAUDIO */
#ifdef SOMA_WEBAUDIO
IMPL_SOMA_TEST_SUITE(webaudio)
#endif /* SOMA_WEBAUDIO */
#ifdef SOMA_PIPEWIRE
IMPL_SOMA_TEST_SUITE(pipewire)
#endif /* SOMA_PIPEWIRE */
#ifdef SOMA_PULSEAUDIO
IMPL_SOMA_TEST_SUITE(pulseaudio)
#endif /* SOMA_PULSEAUDIO */
#ifdef SOMA_JACK
IMPL_SOMA_TEST_SUITE(jack)
#endif /* SOMA_JACK */
#ifdef SOMA_ALSA
IMPL_SOMA_TEST_SUITE(alsa)
#endif /* SOMA_ALSA */
IMPL_SOMA_TEST_SUITE(dummy)
