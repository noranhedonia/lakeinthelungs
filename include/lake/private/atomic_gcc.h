#ifndef LAKE_ATOMIC_H
#error The `gcc` atomic header must not be included outside of `lake/bedrock/atomic.h`
#endif

#include <lake/types.h>

enum lake_memory_model {
    lake_memory_model_relaxed = __ATOMIC_RELAXED,
    lake_memory_model_consume = __ATOMIC_CONSUME,
    lake_memory_model_acquire = __ATOMIC_ACQUIRE,
    lake_memory_model_release = __ATOMIC_RELEASE,
    lake_memory_model_acq_rel = __ATOMIC_ACQ_REL,
    lake_memory_model_seq_cst = __ATOMIC_SEQ_CST,
};
#define LAKE_ATOMIC(T) _Atomic T

#define lake_atomic_init(OBJ, VAL) (*(OBJ) = (VAL))

#define lake_atomic_thread_fence    __atomic_thread_fence
#define lake_atomic_signal_fence    __atomic_signal_fence
#define lake_atomic_is_lock_free    __atomic_is_lock_free

#define lake_atomic_write_explicit  __atomic_store_n
#define lake_atomic_read_explicit   __atomic_load_n

#define lake_atomic_compare_exchange_strong_explicit(DST, EXP, SRC, MODEL1, MODEL2) \
    lake_atomic_compare_exchange_explicit(DST, EXP, SRC, false, MODEL1, MODEL2)

#define lake_atomic_compare_exchange_weak_explicit(DST, EXP, SRC, MODEL1, MODEL2) \
    lake_atomic_compare_exchange_explicit(DST, EXP, SRC, true, MODEL1, MODEL2)

#define lake_atomic_compare_exchange_explicit       __atomic_compare_exchange_n
#define lake_atomic_exchange_explicit               __atomic_exchange_n

#define lake_atomic_add_explicit    __atomic_fetch_add
#define lake_atomic_sub_explicit    __atomic_fetch_sub
#define lake_atomic_and_explicit    __atomic_fetch_and
#define lake_atomic_xor_explicit    __atomic_fetch_xor
#define lake_atomic_or_explicit     __atomic_fetch_or
#define lake_atomic_nand_explicit   __atomic_fetch_nand

#define lake_atomic_flag _Atomic bool
#define lake_atomic_flag_init {0}

#define lake_atomic_flag_test_and_set_explicit(FLAG, MODEL) \
    __atomic_test_and_set(FLAG, MODEL)

#define lake_atomic_flag_clear_explicit(FLAG, MODEL) \
    __atomic_clear(FLAG, MODEL)
