#ifndef LAKE_ATOMIC_H
#error The `C11` atomic header must not be included outside of `lake/bedrock/atomic.h`
#endif

#include <lake/bedrock/types.h>
#include <stdatomic.h>

enum lake_memory_model {
    lake_memory_model_relaxed = memory_order_relaxed,
    lake_memory_model_consume = memory_order_consume,
    lake_memory_model_acquire = memory_order_acquire,
    lake_memory_model_release = memory_order_release,
    lake_memory_model_acq_rel = memory_order_acq_rel,
    lake_memory_model_seq_cst = memory_order_seq_cst,
};
#define LAKE_ATOMIC(T) _Atomic T 

#define lake_atomic_init            atomic_init

#define lake_atomic_thread_fence    atomic_thread_fence
#define lake_atomic_signal_fence    atomic_signal_fence
#define lake_atomic_is_lock_free    atomic_is_lock_free

#define lake_atomic_write_explicit  atomic_store_explicit
#define lake_atomic_read_explicit   atomic_load_explicit

#define lake_atomic_compare_exchange_strong_explicit(DST, EXP, SRC, MODEL1, MODEL2) \
    atomic_compare_exchange_strong_explicit(DST, EXP, SRC, MODEL1, MODEL2)

#define lake_atomic_compare_exchange_weak_explicit(DST, EXP, SRC, MODEL1, MODEL2) \
    atomic_compare_exchange_weak_explicit(DST, EXP, SRC, MODEL1, MODEL2)

#define lake_atomic_compare_exchange_explicit(DST, EXP, SRC, WEAK, MODEL1, MODEL2) \
    (weak ? lake_atomic_compare_exchange_weak_explicit(DST, EXP, SRC, MODEL1, MODEL2) \
         : lake_atomic_compare_exchange_strong_explicit(DST, EXP, SRC, MODEL1, MODEL2))

#define lake_atomic_exchange_explicit(RMW, ARG, MODEL) \
    atomic_exchange_explicit(RMW, ARG, MODEL)

#define lake_atomic_add_explicit    atomic_fetch_add_explicit
#define lake_atomic_sub_explicit    atomic_fetch_sub_explicit
#define lake_atomic_and_explicit    atomic_fetch_and_explicit
#define lake_atomic_xor_explicit    atomic_fetch_xor_explicit
#define lake_atomic_or_explicit     atomic_fetch_or_explicit

#define lake_atomic_flag atomic_flag 
#define lake_atomic_flag_init ATOMIC_FLAG_INIT

#define lake_atomic_flag_test_and_set_explicit(FLAG, MODEL) \
    atomic_flag_test_and_set_explicit(FLAG, MODEL);

#define lake_atomic_flag_clear_explicit(FLAG, MODEL) \
    atomic_flag_clear_explicit(FLAG, MODEL);

