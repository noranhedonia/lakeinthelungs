#include <lake/inthelungs.h>

#define  IMPL_BACKEND_STUB(module, var) \
    FN_LAKE_INTERFACE_IMPL(module, var, module##_interface_assembly) { (void)assembly; return nullptr; }

#ifdef HADAL_WIN32
IMPL_BACKEND_STUB(hadal, win32)
#endif /* HADAL_WIN32 */
#ifdef HADAL_COCOA
IMPL_BACKEND_STUB(hadal, cocoa)
#endif /* HADAL_COCOA */
#ifdef HADAL_UIKIT
IMPL_BACKEND_STUB(hadal, uikit)
#endif /* HADAL_UIKIT */
#ifdef HADAL_ANDROID
IMPL_BACKEND_STUB(hadal, android)
#endif /* HADAL_ANDROID */
#ifdef HADAL_HAIKU
IMPL_BACKEND_STUB(hadal, haiku)
#endif /* HADAL_HAIKU */
#ifdef HADAL_HTML5
IMPL_BACKEND_STUB(hadal, html5)
#endif /* HADAL_HTML5 */
#ifdef HADAL_XCB
IMPL_BACKEND_STUB(hadal, xcb)
#endif /* HADAL_XCB */
#ifdef HADAL_KMS
IMPL_BACKEND_STUB(hadal, kms)
#endif /* HADAL_KMS */
IMPL_BACKEND_STUB(hadal, headless)

#ifdef HADEAN_OPENXR
IMPL_BACKEND_STUB(hadean, openxr)
#endif /* HADEAN_OPENXR */
#ifdef HADEAN_WEBXR 
IMPL_BACKEND_STUB(hadean, webxr)
#endif /* HADEAN_WEBXR */
IMPL_BACKEND_STUB(hadean, headless)

#ifdef SOMA_ASIO
IMPL_BACKEND_STUB(soma, asio)
#endif /* SOMA_ASIO */
#ifdef SOMA_WASAPI
IMPL_BACKEND_STUB(soma, wasapi)
#endif /* SOMA_WASAPI */
#ifdef SOMA_XAUDIO2
IMPL_BACKEND_STUB(soma, xaudio2)
#endif /* SOMA_XAUDIO2 */
#ifdef SOMA_COREAUDIO
IMPL_BACKEND_STUB(soma, coreaudio)
#endif /* SOMA_COREAUDIO */
#ifdef SOMA_AAUDIO
IMPL_BACKEND_STUB(soma, aaudio)
#endif /* SOMA_AAUDIO */
#ifdef SOMA_WEBAUDIO
IMPL_BACKEND_STUB(soma, webaudio)
#endif /* SOMA_WEBAUDIO */
#ifdef SOMA_PULSEAUDIO
IMPL_BACKEND_STUB(soma, pulseaudio)
#endif /* SOMA_PULSEAUDIO */
#ifdef SOMA_JACK
IMPL_BACKEND_STUB(soma, jack)
#endif /* SOMA_JACK */
#ifdef SOMA_ALSA
IMPL_BACKEND_STUB(soma, alsa)
#endif /* SOMA_ALSA */
IMPL_BACKEND_STUB(soma, dummy)

#ifdef MOON_D3D12
IMPL_BACKEND_STUB(moon, d3d12)
#endif /* MOON_D3D12 */
#ifdef MOON_METAL
IMPL_BACKEND_STUB(moon, metal)
#endif /* MOON_METAL */
#ifdef MOON_WEBGPU
IMPL_BACKEND_STUB(moon, webgpu)
#endif /* MOON_WEBGPU */
IMPL_BACKEND_STUB(moon, mock)

u32 moon_calculate_score_from_device_details(moon_device_details const *details)
{
    u32 score = 1;
    if (details->device_type == moon_device_type_discrete_gpu)
        score = 10000;
    else if (details->device_type == moon_device_type_virtual_gpu)
        score = 1000;
    else if (details->device_type == moon_device_type_integrated_gpu)
        score = 100;
    else if (details->device_type == moon_device_type_cpu)
        score = 10;
    
    /* calculate a detailed score maybe?? */
    return score;
}

char const *moon_queue_type_to_string(moon_queue_type type)
{
    switch (type) {
        case moon_queue_type_main: return "main";
        case moon_queue_type_compute: return "compute";
        case moon_queue_type_transfer: return "transfer";
        case moon_queue_type_sparse_binding: return "sparse";
        case moon_queue_type_video_decode: return "decode";
        case moon_queue_type_video_encode: return "encode";
        default: return "unknown";
    }
}

FN_MOON_SURFACE_FORMAT_SELECTOR(moon_default_surface_format_selector)
{
    moon_format const priority[] = {
        moon_format_r8g8b8a8_unorm,
        moon_format_r8g8b8a8_srgb,
        moon_format_b8g8r8a8_unorm,
        moon_format_b8g8r8a8_srgb,
        moon_format_a2b10g10r10_unorm_pack32,
        moon_format_a2r10g10b10_unorm_pack32,
    };
    u32 const priority_count = lake_arraysize(priority);

    for (u32 i = 0; i < priority_count; i++) {
        moon_format const format = priority[i];

        for (u32 j = 0; j < format_count; j++)
            if (formats[j] == format) return (s32)j;
    }
    return -1;
}
