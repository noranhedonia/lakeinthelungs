#include "../framework.h"

s32 SomaImpl_todo(void *) 
{
    return TEST_RESULT_SKIPPED;
}

#define IMPL_SOMA_TEST_SUITE(VAR) \
    FN_TEST_SUITE_INIT(SomaImpl_##VAR) \
    { \
        soma_interface soma = { .impl = soma_interface_impl_##VAR(framework) }; \
        if (soma.impl == nullptr) { \
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
