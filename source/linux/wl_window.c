#include "wl_hadal.h"
#ifdef HADAL_WAYLAND

FN_HADAL_WINDOW_ASSEMBLY(wayland)
{
    (void)hadal;
    (void)assembly;
    (void)out_window;
    return LAKE_ERROR_FEATURE_NOT_PRESENT;
}

FN_HADAL_WINDOW_ZERO_REFCNT(wayland)
{
    (void)window;
}
#endif /* HADAL_WAYLAND */
