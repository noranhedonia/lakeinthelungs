#include "main.h"

FN_SORCERESS_STAGE_GAMEPLAY(lungs)
{
    /* TODO debug exit, delete later */
    if (work->header.timeline >= 2048) 
        work->header.control |= sorceress_control_flag_should_exit;
}
