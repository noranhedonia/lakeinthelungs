#include <lake/sorceress.h>

FN_LAKE_WORK(sorceress_pipeline_gameplay, sorceress_pipeline_work *work)
{
    bool const do_primary_viewport = work->encore->video.primary_viewport.window.v != nullptr;
    bool const do_work =
        do_primary_viewport;

    if (!do_work) return;

    /* TODO process window events */
}
