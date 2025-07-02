#ifndef DECL_TEST_SUITE
#define DECL_TEST_SUITE(SUITE)
#endif

/* bedrock */
DECL_TEST_SUITE(Bedrock_defer)
DECL_TEST_SUITE(Bedrock_drifter)
DECL_TEST_SUITE(Bedrock_file_system)
DECL_TEST_SUITE(Bedrock_job_system)
DECL_TEST_SUITE(Bedrock_machina)
DECL_TEST_SUITE(Bedrock_magic)
DECL_TEST_SUITE(Bedrock_network)
DECL_TEST_SUITE(Bedrock_simd)
DECL_TEST_SUITE(Bedrock_tagged_heap)
DECL_TEST_SUITE(Bedrock_truetype)

/* data structures */
DECL_TEST_SUITE(DS_arena)
DECL_TEST_SUITE(DS_bitset)
DECL_TEST_SUITE(DS_balloc)
DECL_TEST_SUITE(DS_dagraph)
DECL_TEST_SUITE(DS_darray)
DECL_TEST_SUITE(DS_deque)
DECL_TEST_SUITE(DS_hashmap)
DECL_TEST_SUITE(DS_map)
DECL_TEST_SUITE(DS_mpmc)
DECL_TEST_SUITE(DS_sparse)
DECL_TEST_SUITE(DS_stack)
DECL_TEST_SUITE(DS_strbuf)
DECL_TEST_SUITE(DS_switch)

/* math */
DECL_TEST_SUITE(Math_bits)
DECL_TEST_SUITE(Math_camera)
DECL_TEST_SUITE(Math_crypto)
DECL_TEST_SUITE(Math_matrix)
DECL_TEST_SUITE(Math_quaternion)
DECL_TEST_SUITE(Math_trigonometry)
DECL_TEST_SUITE(Math_vector)

/* display backend implementations */
#ifdef HADAL_WIN32
DECL_TEST_SUITE(HadalImpl_win32)
#endif /* HADAL_WIN32 */
#ifdef HADAL_COCOA
DECL_TEST_SUITE(HadalImpl_cocoa)
#endif /* HADAL_COCOA */
#ifdef HADAL_UIKIT
DECL_TEST_SUITE(HadalImpl_uikit)
#endif /* HADAL_UIKIT */
#ifdef HADAL_ANDROID
DECL_TEST_SUITE(HadalImpl_android)
#endif /* HADAL_ANDROID */
#ifdef HADAL_HAIKU
DECL_TEST_SUITE(HadalImpl_haiku)
#endif /* HADAL_HAIKU */
#ifdef HADAL_HTML5
DECL_TEST_SUITE(HadalImpl_html5)
#endif /* HADAL_HTML5 */
#ifdef HADAL_WAYLAND
DECL_TEST_SUITE(HadalImpl_wayland)
#endif /* HADAL_WAYLAND */
#ifdef HADAL_XCB
DECL_TEST_SUITE(HadalImpl_xcb)
#endif /* HADAL_XCB */
#ifdef HADAL_KMS
DECL_TEST_SUITE(HadalImpl_kms)
#endif /* HADAL_KMS */
DECL_TEST_SUITE(HadalImpl_headless)

/* xr backend implementations */
#ifdef HADEAN_OPENXR
DECL_TEST_SUITE(HadeanImpl_openxr)
#endif /* HADEAN_OPENXR */
#ifdef HADEAN_WEBXR
DECL_TEST_SUITE(HadeanImpl_webxr)
#endif /* HADEAN_WEBXR */
DECL_TEST_SUITE(HadeanImpl_headless)

/* rendering backend implementations */
#ifdef MOON_D3D12
DECL_TEST_SUITE(MoonImpl_d3d12)
#endif /* MOON_D3D12 */
#ifdef MOON_METAL
DECL_TEST_SUITE(MoonImpl_metal)
#endif /* MOON_METAL */
#ifdef MOON_WEBGPU
DECL_TEST_SUITE(MoonImpl_webgpu)
#endif /* MOON_WEBGPU */
#ifdef MOON_VULKAN
DECL_TEST_SUITE(MoonImpl_vulkan)
#endif /* MOON_VULKAN */
DECL_TEST_SUITE(MoonImpl_mock)

/* audio backend implementations */
#ifdef SOMA_ASIO
DECL_TEST_SUITE(SomaImpl_asio)
#endif /* SOMA_ASIO */
#ifdef SOMA_WASAPI
DECL_TEST_SUITE(SomaImpl_wasapi)
#endif /* SOMA_WASAPI */
#ifdef SOMA_XAUDIO2
DECL_TEST_SUITE(SomaImpl_xaudio2)
#endif /* SOMA_XAUDIO2 */
#ifdef SOMA_COREAUDIO
DECL_TEST_SUITE(SomaImpl_coreaudio)
#endif /* SOMA_COREAUDIO */
#ifdef SOMA_AAUDIO
DECL_TEST_SUITE(SomaImpl_aaudio)
#endif /* SOMA_AAUDIO */
#ifdef SOMA_WEBAUDIO
DECL_TEST_SUITE(SomaImpl_webaudio)
#endif /* SOMA_WEBAUDIO */
#ifdef SOMA_PIPEWIRE
DECL_TEST_SUITE(SomaImpl_pipewire)
#endif /* SOMA_PIPEWIRE */
#ifdef SOMA_PULSEAUDIO
DECL_TEST_SUITE(SomaImpl_pulseaudio)
#endif /* SOMA_PULSEAUDIO */
#ifdef SOMA_JACK
DECL_TEST_SUITE(SomaImpl_jack)
#endif /* SOMA_JACK */
#ifdef SOMA_ALSA
DECL_TEST_SUITE(SomaImpl_alsa)
#endif /* SOMA_ALSA */
DECL_TEST_SUITE(SomaImpl_dummy)

/* entity-component-system */
DECL_TEST_SUITE(Riven_core)
DECL_TEST_SUITE(Riven_entity)
DECL_TEST_SUITE(Riven_component)
DECL_TEST_SUITE(Riven_system)
DECL_TEST_SUITE(Riven_archetype)
DECL_TEST_SUITE(Riven_query)

/* physics */
DECL_TEST_SUITE(Volta_todo)

/* animation */
DECL_TEST_SUITE(Ipomoea_todo)

/* ui */
DECL_TEST_SUITE(Lovage_todo)

/* framework */
DECL_TEST_SUITE(Sorceress_todo)

/* audio */
DECL_TEST_SUITE(Audio_dsp)
DECL_TEST_SUITE(Audio_mixer)
DECL_TEST_SUITE(Audio_spatial)
DECL_TEST_SUITE(Audio_synth)

/* graphics */
DECL_TEST_SUITE(Graphics_mgpu_renderer)
DECL_TEST_SUITE(Graphics_pipeline_builder)
DECL_TEST_SUITE(Graphics_render_graph)

#undef DECL_TEST_SUITE
