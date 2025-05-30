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
