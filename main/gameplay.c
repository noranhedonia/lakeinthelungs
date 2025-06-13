#include "a_moonlit_walk.h"

FN_LAKE_WORK(a_moonlit_walk__gameplay, struct pipeline_work *work)
{
    if (work == nullptr) return;

    struct a_moonlit_walk *amw = work->amw;
    (void)amw;

    /* TODO read input, imgui stuff */
}
