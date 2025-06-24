#include "framework.h"

#define IMPL_TEST_SUITE(NAME) \
    FN_TEST_SUITE_INIT(NAME); \
    FN_TEST_SUITE_FINI(NAME);

/* bedrock */
IMPL_TEST_SUITE(Defer);
IMPL_TEST_SUITE(Drifter);
IMPL_TEST_SUITE(JobSystem);
IMPL_TEST_SUITE(TaggedHeap);

/* data structures */
IMPL_TEST_SUITE(Darray);
IMPL_TEST_SUITE(Deque);
IMPL_TEST_SUITE(MpmcRing);
IMPL_TEST_SUITE(Strbuf);

/* development */
IMPL_TEST_SUITE(ImguiTools);
IMPL_TEST_SUITE(SlangTools);

/* math */
IMPL_TEST_SUITE(MathBits);

/* display backend implementations */
#ifdef HADAL_WIN32
IMPL_TEST_SUITE(HadalImpl_win32)
#endif /* HADAL_WIN32 */
#ifdef HADAL_COCOA
IMPL_TEST_SUITE(HadalImpl_cocoa)
#endif /* HADAL_COCOA */
#ifdef HADAL_UIKIT
IMPL_TEST_SUITE(HadalImpl_uikit)
#endif /* HADAL_UIKIT */
#ifdef HADAL_ANDROID
IMPL_TEST_SUITE(HadalImpl_android)
#endif /* HADAL_ANDROID */
#ifdef HADAL_HAIKU
IMPL_TEST_SUITE(HadalImpl_haiku)
#endif /* HADAL_HAIKU */
#ifdef HADAL_HTML5
IMPL_TEST_SUITE(HadalImpl_html5)
#endif /* HADAL_HTML5 */
#ifdef HADAL_WAYLAND
IMPL_TEST_SUITE(HadalImpl_wayland)
#endif /* HADAL_WAYLAND */
#ifdef HADAL_XCB
IMPL_TEST_SUITE(HadalImpl_xcb)
#endif /* HADAL_XCB */
#ifdef HADAL_KMS
IMPL_TEST_SUITE(HadalImpl_kms)
#endif /* HADAL_KMS */
IMPL_TEST_SUITE(HadalImpl_headless)

/* rendering backend implementations */
#ifdef MOON_D3D12
IMPL_TEST_SUITE(MoonImpl_d3d12)
#endif /* MOON_D3D12 */
#ifdef MOON_METAL
IMPL_TEST_SUITE(MoonImpl_metal)
#endif /* MOON_METAL */
#ifdef MOON_WEBGPU
IMPL_TEST_SUITE(MoonImpl_webgpu)
#endif /* MOON_WEBGPU */
#ifdef MOON_VULKAN
IMPL_TEST_SUITE(MoonImpl_vulkan)
#endif /* MOON_VULKAN */
IMPL_TEST_SUITE(MoonImpl_mock)

/* audio backend implementations */
#ifdef SOMA_ASIO
IMPL_TEST_SUITE(SomaImpl_asio)
#endif /* SOMA_ASIO */
#ifdef SOMA_WASAPI
IMPL_TEST_SUITE(SomaImpl_wasapi)
#endif /* SOMA_WASAPI */
#ifdef SOMA_XAUDIO2
IMPL_TEST_SUITE(SomaImpl_xaudio2)
#endif /* SOMA_XAUDIO2 */
#ifdef SOMA_COREAUDIO
IMPL_TEST_SUITE(SomaImpl_coreaudio)
#endif /* SOMA_COREAUDIO */
#ifdef SOMA_AAUDIO
IMPL_TEST_SUITE(SomaImpl_aaudio)
#endif /* SOMA_AAUDIO */
#ifdef SOMA_WEBAUDIO
IMPL_TEST_SUITE(SomaImpl_webaudio)
#endif /* SOMA_WEBAUDIO */
#ifdef SOMA_PIPEWIRE
IMPL_TEST_SUITE(SomaImpl_pipewire)
#endif /* SOMA_PIPEWIRE */
#ifdef SOMA_PULSEAUDIO
IMPL_TEST_SUITE(SomaImpl_pulseaudio)
#endif /* SOMA_PULSEAUDIO */
#ifdef SOMA_JACK
IMPL_TEST_SUITE(SomaImpl_jack)
#endif /* SOMA_JACK */
#ifdef SOMA_ALSA
IMPL_TEST_SUITE(SomaImpl_alsa)
#endif /* SOMA_ALSA */
IMPL_TEST_SUITE(SomaImpl_dummy)
