#pragma once

#include <lake/inthelungs.h>

/** After I figure out some use patterns for my interfaces I may move the details into a public header file. */
struct a_moonlit_walk {
    lake_framework const   *framework;

    hadal_interface         hadal;
    hadal_window_v          window;

    moon_interface          moon;
    moon_device_v           primary_device;

    soma_interface          soma;
};

enum pipeline_stage_hint {
    pipeline_stage_hint_continue = 0,
    pipeline_stage_hint_save_and_exit,
    pipeline_stage_hint_restart_engine,
    pipeline_stage_hint_try_recover,
};

struct pipeline_work {
    enum pipeline_stage_hint    stage_hint;
    u64                         timeline;
    f64                         dt;
    struct a_moonlit_walk      *amw;
    struct pipeline_work const *last_work;
    struct pipeline_work       *next_work;
    lake_work_chain             chain;
};

extern FN_LAKE_WORK(a_moonlit_walk__gameplay, struct pipeline_work *work);
extern FN_LAKE_WORK(a_moonlit_walk__rendering, struct pipeline_work *work);
extern FN_LAKE_WORK(a_moonlit_walk__gpuexec, struct pipeline_work *work);
