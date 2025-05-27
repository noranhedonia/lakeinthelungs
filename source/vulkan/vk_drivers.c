#include "vk_moon.h"
#ifdef MOON_VULKAN

FN_MOON_LIST_DEVICE_DETAILS(vulkan)
{
    (void)moon;
    (void)out_device_count;
    (void)out_details;
}

LAKEAPI FN_LAKE_WORK(moon_interface_assembly_vulkan, moon_interface_assembly const *assembly)
{
    (void)assembly;
}
#endif /* MOON_VULKAN */
