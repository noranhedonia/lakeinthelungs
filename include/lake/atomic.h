#pragma once

/** @file lake/atomic.h
 *  @brief Wrapper for different atomic implementations.
 *
 *  Functions for atomic sequences, access and writes of data that will be shared between CPU threads.
 *  They are necessary to guarantee safe operations in a multithreaded environment, and if used 
 *  correctly (I don't do that I guess) will prevent data races, deadlocks and other pesky issues.
 *  Most modern architectures have a pretty straightforward lockless implementation of atomic operations,
 *  on platforms that don't we'll just simulate the behaviour with simple locks instead. Whenever the 
 *  macro `lake_atomic_is_lock_free(T)` if non-zero, guarantees that the data type is lockless.
 *  The macro `LAKE_ATOMIC(T)` is usedto enforce some core rules of how a variable expected to be 
 *  used in atomic operations should be handled, by both the compiler & CPU. It also serves as a 
 *  hint to the user, that a variable of this type should be accessed atomically.
 *
 *  @enum lake_memory_model
 *      An atomic operation can both constrain code motion and be mapped to hardware instructions 
 *      for synchronization between threads. To which extent this happens is controlled by the memory 
 *      models, which are listed in this enum in approximately ascending order of strength. The descriptions
 *      of each memory model is only meant to roughly illustrate the effects and is not a specification.
 *      C++11 memory models can be seen for precise semantics.
 *
 *      lake_memory_model_relaxed  Implies no inter-thread ordering constraints.
 *      lake_memory_model_consume  Is implemented either as relaxed, or as the stronger ACQUIRE model, depending on the platform.
 *
 *      lake_memory_model_acquire  Creates an inter-thread happens-before constraint from the release (or stronger) 
 *                                 semantic store to this acquire load. Can prevent hoisting of code to before the operation.
 *
 *      lake_memory_model_release  Creates an inter-thread happens-before constraint to acquire (or stronger) 
 *                                 semantic loads that read from this release store. Can prevent sinking of 
 *                                 code to after the operation.
 *
 *      lake_memory_model_acq_rel  Combines the effects of both ACQUIRE and RELEASE models.
 *      lake_memory_model_seq_cst  Enforces total ordering with all other SEQ_CST operations.
 *
 *  The memory model is enforced using different synchronization methods. They are implemented in the following:
 *      lake_atomic_compiler_barrier
 *      lake_atomic_thread_fence
 *      lake_atomic_signal_fence
 *
 *  All atomic operations have an explicit and implicit version. The explicit versions have one more 
 *  argument, that allows to specify a memory model constraint for this operation. The implicit versions 
 *  assume `lake_memory_model_relaxed` (contrary to what standard C11/C++11 atomic headers define !!). 
 *  Additional constraints come from a weak and strong memory model for the read-modify-write compare 
 *  exchange operations. This is only desctinct for platforms with a weak memory ordering, thus on 
 *  platforms like x86 the weak compare exchange gets redefined in the strong memory model strong instead.
 *
 *  The following atomic operations are defined:
 *      lake_atomic_init                May help with correctly initializing new atomic variables.
 *      lake_atomic_write               Atomically write value of src to dst.
 *      lake_atomic_read                Atomically read the value of src, no modify operations.
 *      lake_atomic_compare_exchange    Compares the contents of expected, and if equal writes desired to ptr.
 *      lake_atomic_exchange            Stores the contents of val into ptr. The original value of ptr is returned.
 *      lake_atomic_fetch_add           Atomic add operation, returns old value.
 *      lake_atomic_fetch_sub           Atomic sub operation, returns old value.
 *      lake_atomic_fetch_or            Atomic OR |= operation, returns old value.
 *      lake_atomic_fetch_xor           Atomic XOR ^= operation, returns old value.
 *      lake_atomic_fetch_and           Atomic AND &= operation, returns old value.
 *      lake_atomic_flag_test_and_set   An atomic byte check, used to implement atomic booleans and later spinlocks.
 *      lake_atomic_flag_clear          Zeroes out variable that is either bool or char, after the dereference contains 0.
 */
#include <lake/types.h>

#define _LAKE_ATOMIC_IMPLEMENTATION_CXX     0
#define _LAKE_ATOMIC_IMPLEMENTATION_C11     1
#define _LAKE_ATOMIC_IMPLEMENTATION_CLANG   2
#define _LAKE_ATOMIC_IMPLEMENTATION_GCC     3
#define _LAKE_ATOMIC_IMPLEMENTATION_MSVC    4

#ifndef LAKE_ATOMIC_IMPLEMENTATION
    #ifdef __cplusplus
        #define LAKE_ATOMIC_IMPLEMENTATION _LAKE_ATOMIC_IMPLEMENTATION_CXX
    #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
        #define LAKE_ATOMIC_IMPLEMENTATION _LAKE_ATOMIC_IMPLEMENTATION_C11
    #elif defined(LAKE_CC_CLANG_VERSION) && __has_extension(c_atomic)
        #define LAKE_ATOMIC_IMPLEMENTATION _LAKE_ATOMIC_IMPLEMENTATION_CLANG
    #elif LAKE_CC_GNUC_VERSION_CHECK(4,7,0)
        #define LAKE_ATOMIC_IMPLEMENTATION _LAKE_ATOMIC_IMPLEMENTATION_GNUC
    #elif defined(LAKE_CC_MSVC_VERSION) && defined(__STDC_NO_ATOMICS__)
        #define LAKE_ATOMIC_IMPLEMENTATION _LAKE_ATOMIC_IMPLEMENTATION_MSVC
    #endif /* __cplusplus */
#endif /* LAKE_ATOMIC_IMPLEMENTATION */

#define LAKE_ATOMIC_H
#if LAKE_ATOMIC_IMPLEMENTATION == _LAKE_ATOMIC_IMPLEMENTATION_CXX
    #include <lake/private/atomic_c++.hpp>
#elif LAKE_ATOMIC_IMPLEMENTATION == _LAKE_ATOMIC_IMPLEMENTATION_C11
    #include <lake/private/atomic_c11.h>
#elif LAKE_ATOMIC_IMPLEMENTATION == _LAKE_ATOMIC_IMPLEMENTATION_CLANG
    #include <lake/private/atomic_clang.h>
#elif LAKE_ATOMIC_IMPLEMENTATION == _LAKE_ATOMIC_IMPLEMENTATION_GCC 
    #include <lake/private/atomic_gcc.h>
#elif LAKE_ATOMIC_IMPLEMENTATION == _LAKE_ATOMIC_IMPLEMENTATION_MSVC
    #include <lake/private/atomic_msvc.h>
#else
    #error Platform and compiler not recognized, could not resolve LAKE_ATOMIC_IMPLEMENTATION.
#endif /* LAKE_ATOMIC_IMPLEMENTATION */
#undef LAKE_ATOMIC_H

typedef LAKE_ATOMIC(s8)         atomic_s8;
typedef LAKE_ATOMIC(s16)        atomic_s16;
typedef LAKE_ATOMIC(s32)        atomic_s32;
typedef LAKE_ATOMIC(s64)        atomic_s64;

typedef LAKE_ATOMIC(u8)         atomic_u8;
typedef LAKE_ATOMIC(u16)        atomic_u16;
typedef LAKE_ATOMIC(u32)        atomic_u32;
typedef LAKE_ATOMIC(u64)        atomic_u64;

typedef LAKE_ATOMIC(sptr)       atomic_sptr;
typedef LAKE_ATOMIC(uptr)       atomic_uptr;
typedef LAKE_ATOMIC(ssize)      atomic_ssize;
typedef LAKE_ATOMIC(usize)      atomic_usize;

/** Relaxed atomic operations.
 *
 *  When an operation on an atomic variable is not expected to synchronize 
 *  with operations on other (atomic or non-atomic) variables, no memory 
 *  barriers are needed and the relaxed memory ordering can be used. These 
 *  macros make such uses less daunting, but not invisible. 
 */
#define lake_atomic_write(VAR, VALUE) \
    lake_atomic_write_explicit(VAR, VALUE, lake_memory_model_relaxed)

#define lake_atomic_read(VAR) \
    lake_atomic_read_explicit(VAR, lake_memory_model_relaxed)

#define lake_atomic_compare_exchange(PTR, EXPECTED, DESIRED, WEAK) \
    lake_atomic_compare_exchange_explicit(PTR, EXPECTED, DESIRED, WEAK, \
            lake_memory_model_relaxed, lake_memory_model_relaxed)

#define lake_atomic_exchange(PTR, VALUE, ORIGINAL) \
    lake_atomic_exchange_explicit(PTR, VALUE, ORIGINAL, lake_memory_model_relaxed)

/** These operands always return the value that had previously been in RMW:
 *     { tmp = *ptr; *ptr op= val; return tmp; }
 *     { tmp = *ptr; *ptr = ~(*ptr & val); return tmp; } // nand 
 */
#define lake_atomic_add(RMW, ARG) \
    lake_atomic_add_explicit(RMW, ARG, lake_memory_model_relaxed)

#define lake_atomic_sub(RMW, ARG) \
    lake_atomic_sub_explicit(RMW, ARG, lake_memory_model_relaxed)

#define lake_atomic_and(RMW, ARG) \
    lake_atomic_and_explicit(RMW, ARG, lake_memory_model_relaxed)

#define lake_atomic_xor(RMW, ARG) \
    lake_atomic_xor_explicit(RMW, ARG, lake_memory_model_relaxed)

#define lake_atomic_or(RMW, ARG) \
    lake_atomic_or_explicit(RMW, ARG, lake_memory_model_relaxed)

/** The spinlock is not recursive. */
typedef struct {
    lake_atomic_flag flag;
} lake_spinlock;
#define lake_spinlock_init {lake_atomic_flag_init}

LAKE_FORCE_INLINE bool lake_spinlock_try_acquire(lake_spinlock volatile *lock)
{ return lake_atomic_flag_test_and_set_explicit(&lock->flag, lake_memory_model_acquire); }

LAKE_FORCE_INLINE void lake_spinlock_acquire(lake_spinlock volatile *lock)
{ do { /* spin */ } while(lake_spinlock_try_acquire(lock)); }

LAKE_FORCE_INLINE void lake_spinlock_release(lake_spinlock volatile *lock)
{ lake_atomic_flag_clear_explicit(&lock->flag, lake_memory_model_release); }
