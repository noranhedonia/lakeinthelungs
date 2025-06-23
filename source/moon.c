#include <lake/moon.h>

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

FN_LAKE_INTERFACE_IMPL(moon, mock, lake_framework)
{
    (void)assembly;
    return nullptr;
}
