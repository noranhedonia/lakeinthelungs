#ifndef LAKE_ATOMIC_H
#error The `C++` atomic header must not be included outside of `lake/bedrock/atomic.h`
#endif

#include <lake/bedrock/types.h>
#include <atomic>

#define lake_memory_model_relaxed std::memory_order_relaxed
#define lake_memory_model_consume std::memory_order_consume
#define lake_memory_model_acquire std::memory_order_acquire
#define lake_memory_model_release std::memory_order_release
#define lake_memory_model_acq_rel std::memory_order_acq_rel
#define lake_memory_model_seq_cst std::memory_order_seq_cst

#define LAKE_ATOMIC(T) std::atomic<T>

#define lake_atomic_init         std::atomic_init
#define lake_atomic_thread_fence std::atomic_thread_fence
#define lake_atomic_signal_fence std::atomic_signal_fence
#define lake_atomic_is_lock_free std::atomic_is_lock_free

#define lake_atomic_write_explicit std::atomic_store_explicit
#define lake_atomic_read_explicit std::atomic_load_explicit

#define lake_atomic_compare_exchange_strong_explicit(DST, EXP, SRC, MODEL1, MODEL2) \
    std::atomic_compare_exchange_strong_explicit(DST, EXP, SRC, MODEL1, MODEL2)

#define lake_atomic_compare_exchange_weak_explicit(DST, EXP, SRC, MODEL1, MODEL2) \
    std::atomic_compare_exchange_weak_explicit(DST, EXP, SRC, MODEL1, MODEL2)

#define lake_atomic_compare_exchange_explicit(DST, EXP, SRC, WEAK, MODEL1, MODEL2) \
    (WEAK ? lake_atomic_compare_exchange_weak_explicit(DST, EXP, SRC, MODEL1, MODEL2) \
         : lake_atomic_compare_exchange_strong_explicit(DST, EXP, SRC, MODEL1, MODEL2))

#define lake_atomic_exchange_explicit(RMW, ARG, MODEL) \
    std::atomic_exchange_explicit(RMW, ARG, MODEL)

#define lake_atomic_add_explicit(RMW, ARG, MODEL) \
    std::atomic_fetch_add_explicit(RMW, ARG, MODEL)
#define lake_atomic_sub_explicit(RMW, ARG, MODEL) \
    std::atomic_fetch_sub_explicit(RMW, ARG, MODEL)
#define lake_atomic_and_explicit(RMW, ARG, MODEL) \
    std::atomic_fetch_and_explicit(RMW, ARG, MODEL)
#define lake_atomic_xor_explicit(RMW, ARG, MODEL) \
    std::atomic_fetch_xor_explicit(RMW, ARG, MODEL)
#define lake_atomic_or_explicit(RMW, ARG, MODEL) \
    std::atomic_fetch_or_explicit(RMW, ARG, MODEL)

template <typename T>
T lake_atomic_nand_explicit(std::atomic<T>& atomic_obj, T value, std::memory_order order = std::memory_order_seq_cst) {
    T current_value = atomic_obj.load(order);
    T new_value = ~(current_value & value);
    while (!atomic_obj.compare_exchange_weak(current_value, new_value, order)) {
        new_value = ~(current_value & value);
    }
    return current_value;
}

#define lake_atomic_flag std::atomic_flag
#define lake_atomic_flag_init ATOMIC_FLAG_INIT

#define lake_atomic_flag_test_and_set_explicit(FLAG, MODEL) \
    std::atomic_flag_test_and_set_explicit(FLAG, MODEL)

#define lake_atomic_flag_clear_explicit(FLAG, MODEL) \
    std::atomic_flag_clear_explicit(FLAG, MODEL)
