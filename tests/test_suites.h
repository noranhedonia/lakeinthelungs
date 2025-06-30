#include "test_framework.h"

#define IMPL_TEST_SUITE(NAME) \
    FN_TEST_SUITE_INIT(NAME); \
    FN_TEST_SUITE_FINI(NAME);

/* bedrock */
IMPL_TEST_SUITE(Bedrock_defer);
IMPL_TEST_SUITE(Bedrock_drifter);
IMPL_TEST_SUITE(Bedrock_file_system);
IMPL_TEST_SUITE(Bedrock_job_system);
IMPL_TEST_SUITE(Bedrock_machina);
IMPL_TEST_SUITE(Bedrock_network);
IMPL_TEST_SUITE(Bedrock_tagged_heap);
IMPL_TEST_SUITE(Bedrock_truetype);

/* data structures */
IMPL_TEST_SUITE(DS_arena_allocator);
IMPL_TEST_SUITE(DS_bitset);
IMPL_TEST_SUITE(DS_block_allocator);
IMPL_TEST_SUITE(DS_dagraph);
IMPL_TEST_SUITE(DS_darray);
IMPL_TEST_SUITE(DS_deque);
IMPL_TEST_SUITE(DS_hashmap);
IMPL_TEST_SUITE(DS_map);
IMPL_TEST_SUITE(DS_mpmc_ring);
IMPL_TEST_SUITE(DS_sparse_set);
IMPL_TEST_SUITE(DS_stack_allocator);
IMPL_TEST_SUITE(DS_strbuf);
IMPL_TEST_SUITE(DS_switch_list);

/* math */
IMPL_TEST_SUITE(Math_bits);
IMPL_TEST_SUITE(Math_camera);
IMPL_TEST_SUITE(Math_matrix);
IMPL_TEST_SUITE(Math_quaternion);
IMPL_TEST_SUITE(Math_simd);
IMPL_TEST_SUITE(Math_trigonometry);
IMPL_TEST_SUITE(Math_vector);

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

/* xr backend implementations */
#ifdef HADEAN_OPENXR
IMPL_TEST_SUITE(HadeanImpl_openxr)
#endif /* HADEAN_OPENXR */
#ifdef HADEAN_WEBXR
IMPL_TEST_SUITE(HadeanImpl_webxr)
#endif /* HADEAN_WEBXR */
IMPL_TEST_SUITE(HadeanImpl_headless)

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

/* entity-component-system */
IMPL_TEST_SUITE(Riven_core)
IMPL_TEST_SUITE(Riven_entity)
IMPL_TEST_SUITE(Riven_component)
IMPL_TEST_SUITE(Riven_system)
IMPL_TEST_SUITE(Riven_archetype)
IMPL_TEST_SUITE(Riven_query)

/* physics */
IMPL_TEST_SUITE(Volta_todo)

/* animation */
IMPL_TEST_SUITE(Ipomoea_todo)

/* ui */
IMPL_TEST_SUITE(Lovage_todo)

/* framework */
IMPL_TEST_SUITE(Sorceress_todo)

/* audio */
IMPL_TEST_SUITE(Audio_dsp)
IMPL_TEST_SUITE(Audio_mixer)
IMPL_TEST_SUITE(Audio_spatial)
IMPL_TEST_SUITE(Audio_synth)

/* graphics */
IMPL_TEST_SUITE(Graphics_pipeline_builder)
IMPL_TEST_SUITE(Graphics_render_graph)
IMPL_TEST_SUITE(Graphics_renderer)
