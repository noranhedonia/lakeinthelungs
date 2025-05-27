#include "internal.h"

u32 lake_worker_thread_index(void)
{
    return 0; 
}

lake_work_chain lake_acquire_chain_(usize initial_value)
{
    (void)initial_value;
    return nullptr;
}

void lake_submit_work(
    u32                      work_count, 
    lake_work_details const *work, 
    lake_work_chain         *out_chain)
{
    (void)work_count;
    (void)work;
    (void)out_chain;
}

void lake_yield(lake_work_chain chain)
{
    (void)chain;
}
