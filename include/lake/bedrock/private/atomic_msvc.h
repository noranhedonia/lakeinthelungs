#ifndef LAKE_ATOMIC_H
#error The `msvc` atomic header must not be included outside of `lake/bedrock/atomic.h`
#endif

#include <lake/bedrock/types.h>
#include <intrin.h>

#if UINTPTR_MAX == 0xffffffffffffffffull
    #define __intptr __int64
    #define __ptr s64
#elif UINTPTR_MAX == 0xffffffffu
    #define __intptr __int32
    #define __ptr s32
#else
    #error Unable to determine pointer width.
#endif

enum lake_memory_model {
    lake_memory_model_relaxed = 0,
    lake_memory_model_consume = 1,
    lake_memory_model_acquire = 2,
    lake_memory_model_release = 3,
    lake_memory_model_acq_rel = 4,
    lake_memory_model_seq_cst = 5,
};
#define LAKE_ATOMIC(T) volatile T

#define lake_atomic_init(OBJ, VAL) (*(OBJ) = (VAL))

#define lake_atomic_is_lock_free(obj) \
    ((void)*(obj), (sizeof((*(obj)) ? 2 : 0) <= 8 && lake_is_pow2(sizeof((*(obj)) ? 2 : 0))

LAKE_FORCE_INLINE void 
_lake_atomic_msvc_compiler_barrier(enum lake_memory_model model)
{
    /** In case of 'lake_memory_model_consume', it is implicitly assumed that 
     *  the compiler will not move instructions that have data-dependency 
     *  on the variable in question before the barrier. */
    if (model > lake_memory_model_consume) 
        _ReadWriteBarrier();
}

LAKE_FORCE_INLINE void
lake_atomic_thread_fence(enum lake_memory_model model)
{
    /* x86 is strongly ordered and acquire/release semantics come automatically */
    _lake_atomic_msvc_compiler_barrier(model);
    if (model == lake_memory_model_seq_cst) {
        MemoryBarrier();
        _lake_atomic_msvc_compiler_barrier(model);
    }
}

LAKE_FORCE_INLINE void 
lake_atomic_signal_fence(enum lake_memory_model model)
{ _lake_atomic_msvc_compiler_barrier(model); }

LAKE_FORCE_INLINE __int8 _lake_atomic_msvc_xchg8(__int8 volatile *addr, __int8 val) 
{ return _InterlockedExchange8((__int8 volatile *)addr, val); }
LAKE_FORCE_INLINE __int16 _lake_atomic_msvc_xchg16(__int16 volatile *addr, __int16 val) 
{ return _InterlockedExchange16((__int16 volatile *)addr, val); }
LAKE_FORCE_INLINE __int32 _lake_atomic_msvc_xchg32(__int32 volatile *addr, __int32 val) 
{ return _InterlockedExchange((__int32 volatile *)addr, val); }
LAKE_FORCE_INLINE __int64 _lake_atomic_msvc_xchg64(__int64 volatile *addr, __int64 val) 
{ return _InterlockedExchange64((__int64 volatile *)addr, val); }
#define _lake_atomic_msvc_xchg_ptr(PTR) _lake_atomic_msvc_xchg##PTR

#define _LAKE_ATOMIC_MSVC_EXCHANGE(T, SIZE)                             \
    LAKE_FORCE_INLINE T                                                 \
    _lake_atomic_msvc_exchange_##T(LAKE_ATOMIC(T) *obj, T desired) {    \
        return (T)_lake_atomic_msvc_xchg##SIZE(                         \
            (LAKE_CONCAT2(__int, SIZE) volatile *)obj,                  \
            (LAKE_CONCAT2(__int, SIZE))desired);                        \
    }
_LAKE_ATOMIC_MSVC_EXCHANGE(s8,  8)
_LAKE_ATOMIC_MSVC_EXCHANGE(s16, 16)
_LAKE_ATOMIC_MSVC_EXCHANGE(s32, 32)
_LAKE_ATOMIC_MSVC_EXCHANGE(s64, 64)
_LAKE_ATOMIC_MSVC_EXCHANGE(u8,  8)
_LAKE_ATOMIC_MSVC_EXCHANGE(u16, 16)
_LAKE_ATOMIC_MSVC_EXCHANGE(u32, 32)
_LAKE_ATOMIC_MSVC_EXCHANGE(u64, 64)
LAKE_FORCE_INLINE void *_lake_atomic_msvc_exchange_ptr(LAKE_ATOMIC(void *)*obj, void *desired) 
{ return (void *)_lake_atomic_msvc_xchg_ptr(__ptr)((__intptr volatile *)obj, (ssize)desired); }

#define lake_atomic_exchange_explicit(OBJ, DESIRED, MODEL)      \
    _Generic((OBJ),                                             \
        LAKE_ATOMIC(s8)*: _lake_atomic_msvc_exchange_s8,        \
        LAKE_ATOMIC(s16)*: _lake_atomic_msvc_exchange_s16,      \
        LAKE_ATOMIC(s32)*: _lake_atomic_msvc_exchange_s32,      \
        LAKE_ATOMIC(s64)*: _lake_atomic_msvc_exchange_s64,      \
        LAKE_ATOMIC(u8)*: _lake_atomic_msvc_exchange_u8,        \
        LAKE_ATOMIC(u16)*: _lake_atomic_msvc_exchange_u16,      \
        LAKE_ATOMIC(u32)*: _lake_atomic_msvc_exchange_u32,      \
        LAKE_ATOMIC(u64)*: _lake_atomic_msvc_exchange_u64,      \
        LAKE_ATOMIC(void *)*: _lake_atomic_msvc_exchange_ptr    \
    )(OBJ, DESIRED)

#define lake_atomic_write_explicit(OBJ, VAL, MODEL)
    lake_atomic_exchange_explicit(OBJ, VAL, MODEL)

LAKE_FORCE_INLINE __int8 _lake_atomic_msvc_cmpxchg8(__int8 volatile *addr, __int8 oldval, __int8 newval) 
{ return _InterlockedCompareExchange8((__int8 volatile *)addr, newval, oldval); }
LAKE_FORCE_INLINE __int16 _lake_atomic_msvc_cmpxchg16(__int16 volatile *addr, __int16 oldval, __int16 newval) 
{ return _InterlockedCompareExchange16((__int16 volatile *)addr, newval, oldval); }
LAKE_FORCE_INLINE __int32 _lake_atomic_msvc_cmpxchg32(__int32 volatile *addr, __int32 oldval, __int32 newval) 
{ return _InterlockedCompareExchange((__int32 volatile *)addr, newval, oldval); }
LAKE_FORCE_INLINE __int64 _lake_atomic_msvc_cmpxchg64(__int64 volatile *addr, __int64 oldval, __int64 newval) 
{ return _InterlockedCompareExchange64((__int64 volatile *)addr, newval, oldval); }
#define _lake_atomic_msvc_cmpxchg_ptr(PTR) _lake_atomic_msvc_cmpxchg##PTR

#define _LAKE_ATOMIC_MSVC_COMPARE_EXCHANGE(T, SIZE)                                         \
    LAKE_FORCE_INLINE bool                                                                  \
    _lake_atomic_msvc_compare_exchange_##T(LAKE_ATOMIC(T) *obj, T *expected, T desired) {   \
        T cmp = *expected, val = _lake_atomic_msvc_cmpxchg##SIZE(                           \
                (__int##SIZE volatile *)obj,                                                \
                (__int##SIZE)cmp,                                                           \
                (__int##SIZE)desired);                                                      \
        return (bool)(val == cmp);                                                          \
    }
_LAKE_ATOMIC_MSVC_COMPARE_EXCHANGE(s8,  8)
_LAKE_ATOMIC_MSVC_COMPARE_EXCHANGE(s16, 16)
_LAKE_ATOMIC_MSVC_COMPARE_EXCHANGE(s32, 32)
_LAKE_ATOMIC_MSVC_COMPARE_EXCHANGE(s64, 64)
_LAKE_ATOMIC_MSVC_COMPARE_EXCHANGE(u8,  8)
_LAKE_ATOMIC_MSVC_COMPARE_EXCHANGE(u16, 16)
_LAKE_ATOMIC_MSVC_COMPARE_EXCHANGE(u32, 32)
_LAKE_ATOMIC_MSVC_COMPARE_EXCHANGE(u64, 64)
LAKE_FORCE_INLINE bool _lake_atomic_msvc_compare_exchange_ptr(LAKE_ATOMIC(void *)*obj, void **expected, void *desired) { 
    void *cmp = *expected, val = _lake_atomic_msvc_cmpxchg_ptr(__ptr)(
        (__intptr volatile *)obj, (ptrdiff_t)cmp, (ptrdiff_t)desired);
    return (bool)(ssize)(val == cmp);
}

#define lake_atomic_compare_exchange_strong_explicit(DST, EXP, SRC, MODEL1, MODEL2) \
    _Generic((DST),                                                                 \
        LAKE_ATOMIC(s8)*: _lake_atomic_msvc_compare_exchange_s8,                    \
        LAKE_ATOMIC(s16)*: _lake_atomic_msvc_compare_exchange_s16,                  \
        LAKE_ATOMIC(s32)*: _lake_atomic_msvc_compare_exchange_s32,                  \
        LAKE_ATOMIC(s64)*: _lake_atomic_msvc_compare_exchange_s64,                  \
        LAKE_ATOMIC(u8)*: _lake_atomic_msvc_compare_exchange_u8,                    \
        LAKE_ATOMIC(u16)*: _lake_atomic_msvc_compare_exchange_u16,                  \
        LAKE_ATOMIC(u32)*: _lake_atomic_msvc_compare_exchange_u32,                  \
        LAKE_ATOMIC(u64)*: _lake_atomic_msvc_compare_exchange_u64,                  \
        LAKE_ATOMIC(void *)*: _lake_atomic_msvc_compare_exchange_ptr                \
    )(DST, EXP, SRC)

#define lake_atomic_compare_exchange_weak_explicit(DST, EXP, SRC, WEAK, MODEL1, MODEL2)
    lake_atomic_compare_exchange_strong_explicit(DST, EXP, SRC, MODEL1, MODEL2)   

#define lake_atomic_compare_exchange_explicit(DST, EXP, SRC, WEAK, MODEL1, MODEL2)
    lake_atomic_compare_exchange_strong_explicit(DST, EXP, SRC, MODEL1, MODEL2)   

LAKE_FORCE_INLINE __int8 _lake_atomic_msvc_xadd8(__int8 volatile *addr, __int8 val)
{ return _InterlockedExchangeAdd8(addr, val); }
LAKE_FORCE_INLINE __int16 _lake_atomic_msvc_xadd16(__int16 volatile *addr, __int16 val)
{ return _InterlockedExchangeAdd16(addr, val); }
LAKE_FORCE_INLINE __int32 _lake_atomic_msvc_xadd32(__int32 volatile *addr, __int32 val)
{ return _InterlockedExchangeAdd(addr, val); }
LAKE_FORCE_INLINE __int64 _lake_atomic_msvc_xadd64(__int64 volatile *addr, __int64 val)
{ return _InterlockedExchangeAdd64(addr, val); }
#define _lake_atomic_msvc_xadd_ptr(PTR) LAKE_CONCAT2(_lake_atomic_msvc_xadd_, PTR)

#define _LAKE_ATOMIC_MSVC_ADD(T, SIZE)                      \
    LAKE_FORCE_INLINE T                                     \
    _lake_atomic_msvc_add_##T(LAKE_ATOMIC(T) *obj, T arg) { \
        return (T)_lake_atomic_msvc_xadd##SIZE(             \
                (__int##SIZE volatile *)obj, T arg);        \
    }
_LAKE_ATOMIC_MSVC_ADD(s8,  8)
_LAKE_ATOMIC_MSVC_ADD(s16, 16)
_LAKE_ATOMIC_MSVC_ADD(s32, 32)
_LAKE_ATOMIC_MSVC_ADD(s64, 64)
_LAKE_ATOMIC_MSVC_ADD(u8,  8)
_LAKE_ATOMIC_MSVC_ADD(u16, 16)
_LAKE_ATOMIC_MSVC_ADD(u32, 32)
_LAKE_ATOMIC_MSVC_ADD(u64, 64)
LAKE_FORCE_INLINE void *_lake_atomic_msvc_add_ptr(LAKE_ATOMIC(void *)*obj, void *arg) {
    return (void *)_lake_atomic_msvc_xadd_ptr(__ptr)((__intptr volatile *)obj, (__intptr)arg);
}

#define lake_atomic_add_explicit(RMW, ARG, MODEL)           \
    _Generic((RMW),                                         \
        LAKE_ATOMIC(s8)*: _lake_atomic_msvc_add_s8,         \
        LAKE_ATOMIC(s16)*: _lake_atomic_msvc_add_s16,       \
        LAKE_ATOMIC(s32)*: _lake_atomic_msvc_add_s32,       \
        LAKE_ATOMIC(s64)*: _lake_atomic_msvc_add_s64,       \
        LAKE_ATOMIC(u8)*: _lake_atomic_msvc_add_u8,         \
        LAKE_ATOMIC(u16)*: _lake_atomic_msvc_add_u16,       \
        LAKE_ATOMIC(u32)*: _lake_atomic_msvc_add_u32,       \
        LAKE_ATOMIC(u64)*: _lake_atomic_msvc_add_u64,       \
        LAKE_ATOMIC(void *)*: _lake_atomic_msvc_add_ptr     \
    )(RMW, ARG)
#define lake_atomic_sub_explicit(RMW, ARG, MODEL) \
    lake_atomic_add_explicit(RMW, -(ARG), MODEL)

#define _LAKE_ATOMIC_MSVC_READ(T, SIZE) \
    LAKE_FORCE_INLINE T \
    _lake_atomic_msvc_read_##T (LAKE_ATOMIC(T) *obj) \
    { T val; _ReadBarrier(); val = *obj; _ReadWriteBarrier(); return val; }
_LAKE_ATOMIC_MSVC_READ(s8,  8)
_LAKE_ATOMIC_MSVC_READ(s16, 16)
_LAKE_ATOMIC_MSVC_READ(s32, 32)
_LAKE_ATOMIC_MSVC_READ(s64, 64)
_LAKE_ATOMIC_MSVC_READ(u8,  8)
_LAKE_ATOMIC_MSVC_READ(u16, 16)
_LAKE_ATOMIC_MSVC_READ(u32, 32)
_LAKE_ATOMIC_MSVC_READ(u64, 64)
LAKE_FORCE_INLINE void *_lake_atomic_msvc_read_ptr(void **obj) {
    void *val; _ReadBarrier(); val = *obj; _ReadWriteBarrier(); return val;
}

#define lake_atomic_read_explicit(OBJ, MODEL)               \
    _Generic((OBJ),                                         \
        LAKE_ATOMIC(s8)*: _lake_atomic_msvc_read_s8,        \
        LAKE_ATOMIC(s16)*: _lake_atomic_msvc_read_s16,      \
        LAKE_ATOMIC(s32)*: _lake_atomic_msvc_read_s32,      \
        LAKE_ATOMIC(s64)*: _lake_atomic_msvc_read_s64,      \
        LAKE_ATOMIC(u8)*: _lake_atomic_msvc_read_u8,        \
        LAKE_ATOMIC(u16)*: _lake_atomic_msvc_read_u16,      \
        LAKE_ATOMIC(u32)*: _lake_atomic_msvc_read_u32,      \
        LAKE_ATOMIC(u64)*: _lake_atomic_msvc_read_u64,      \
        LAKE_ATOMIC(void *)*: _lake_atomic_msvc_read_ptr    \
    )(OBJ)

#define _LAKE_ATOMIC_MSVC_OP(PREFIX, T, SIZE, OP)                                       \
    LAKE_FORCE_INLINE T                                                                 \
    _lake_atomic_msvc_##PREFIX##T(LAKE_ATOMIC(T) *obj, T arg) {                         \
        T oldval, newval;                                                               \
        do {                                                                            \
            oldval = lake_atomic_read_explicit(obj, lake_memory_model_acq_rel);         \
            newval = oldval OP arg;                                                     \
        } while (!lake_atomic_compare_exchange_strong_explicit(obj, &oldval, newval));  \
        return oldval;                                                                  \
    }

#define _LAKE_ATOMIC_MSVC_OP_PTR(PREFIX, OP)                                                            \
    LAKE_FORCE_INLINE void *                                                                            \
    _lake_atomic_msvc_##PREFIX##ptr(LAKE_ATOMIC(void *)*obj, void *arg) {                               \
        ssize oldval, newval;                                                                           \
        do {                                                                                            \
            oldval = (ssize)lake_atomic_read_explicit(obj, lake_memory_model_acq_rel);                  \
            newval = oldval OP (ssize)arg;                                                              \
        } while (!lake_atomic_compare_exchange_strong_explicit(obj, (void **)&oldval, (void *)newval)); \
        return (void *)oldval;                                                                          \
    }

/* and */
_LAKE_ATOMIC_MSVC_OP(and_, s8,  &)
_LAKE_ATOMIC_MSVC_OP(and_, s16, &)
_LAKE_ATOMIC_MSVC_OP(and_, s32, &)
_LAKE_ATOMIC_MSVC_OP(and_, s64, &)
_LAKE_ATOMIC_MSVC_OP(and_, u8,  &)
_LAKE_ATOMIC_MSVC_OP(and_, u16, &)
_LAKE_ATOMIC_MSVC_OP(and_, u32, &)
_LAKE_ATOMIC_MSVC_OP(and_, u64, &)
_LAKE_ATOMIC_MSVC_OP_PTR(and_,  &)

#define lake_atomic_and_explicit(RMW, ARG, MODEL)           \
    _Generic((RMW),                                         \
        LAKE_ATOMIC(s8)*: _lake_atomic_msvc_and_s8,         \
        LAKE_ATOMIC(s16)*: _lake_atomic_msvc_and_s16,       \
        LAKE_ATOMIC(s32)*: _lake_atomic_msvc_and_s32,       \
        LAKE_ATOMIC(s64)*: _lake_atomic_msvc_and_s64,       \
        LAKE_ATOMIC(u8)*: _lake_atomic_msvc_and_u8,         \
        LAKE_ATOMIC(u16)*: _lake_atomic_msvc_and_u16,       \
        LAKE_ATOMIC(u32)*: _lake_atomic_msvc_and_u32,       \
        LAKE_ATOMIC(u64)*: _lake_atomic_msvc_and_u64,       \
        LAKE_ATOMIC(void *)*: _lake_atomic_msvc_and_ptr     \
    )(RMW, ARG)

/* xor */
_LAKE_ATOMIC_MSVC_OP(xor_, s8,  ^)
_LAKE_ATOMIC_MSVC_OP(xor_, s16, ^)
_LAKE_ATOMIC_MSVC_OP(xor_, s32, ^)
_LAKE_ATOMIC_MSVC_OP(xor_, s64, ^)
_LAKE_ATOMIC_MSVC_OP(xor_, u8,  ^)
_LAKE_ATOMIC_MSVC_OP(xor_, u16, ^)
_LAKE_ATOMIC_MSVC_OP(xor_, u32, ^)
_LAKE_ATOMIC_MSVC_OP(xor_, u64, ^)
_LAKE_ATOMIC_MSVC_OP_PTR(xor_,  ^)

#define lake_atomic_xor_explicit(RMW, ARG, MODEL)           \
    _Generic((RMW),                                         \
        LAKE_ATOMIC(s8)*: _lake_atomic_msvc_xor_s8,         \
        LAKE_ATOMIC(s16)*: _lake_atomic_msvc_xor_s16,       \
        LAKE_ATOMIC(s32)*: _lake_atomic_msvc_xor_s32,       \
        LAKE_ATOMIC(s64)*: _lake_atomic_msvc_xor_s64,       \
        LAKE_ATOMIC(u8)*: _lake_atomic_msvc_xor_u8,         \
        LAKE_ATOMIC(u16)*: _lake_atomic_msvc_xor_u16,       \
        LAKE_ATOMIC(u32)*: _lake_atomic_msvc_xor_u32,       \
        LAKE_ATOMIC(u64)*: _lake_atomic_msvc_xor_u64,       \
        LAKE_ATOMIC(void *)*: _lake_atomic_msvc_xor_ptr     \
    )(RMW, ARG)

/* or */
_LAKE_ATOMIC_MSVC_OP(or_, s8,  |)
_LAKE_ATOMIC_MSVC_OP(or_, s16, |)
_LAKE_ATOMIC_MSVC_OP(or_, s32, |)
_LAKE_ATOMIC_MSVC_OP(or_, s64, |)
_LAKE_ATOMIC_MSVC_OP(or_, u8,  |)
_LAKE_ATOMIC_MSVC_OP(or_, u16, |)
_LAKE_ATOMIC_MSVC_OP(or_, u32, |)
_LAKE_ATOMIC_MSVC_OP(or_, u64, |)
_LAKE_ATOMIC_MSVC_OP_PTR(or_,  |)

#define lake_atomic_or_explicit(RMW, ARG, MODEL)            \
    _Generic((RMW),                                         \
        LAKE_ATOMIC(s8)*: _lake_atomic_msvc_or_s8,          \
        LAKE_ATOMIC(s16)*: _lake_atomic_msvc_or_s16,        \
        LAKE_ATOMIC(s32)*: _lake_atomic_msvc_or_s32,        \
        LAKE_ATOMIC(s64)*: _lake_atomic_msvc_or_s64,        \
        LAKE_ATOMIC(u8)*: _lake_atomic_msvc_or_u8,          \
        LAKE_ATOMIC(u16)*: _lake_atomic_msvc_or_u16,        \
        LAKE_ATOMIC(u32)*: _lake_atomic_msvc_or_u32,        \
        LAKE_ATOMIC(u64)*: _lake_atomic_msvc_or_u64,        \
        LAKE_ATOMIC(void *)*: _lake_atomic_msvc_or_ptr      \
    )(RMW, ARG)

#define lake_atomic_flag bool volatile
#define lake_atomic_flag_init {0}

LAKE_FORCE_INLINE bool lake_atomic_flag_test_and_set_explicit(bool volatile *flag, enum lake_memory_model model)
{
    s8 o = false;
    return lake_atomic_compare_exchange_strong_explicit((LAKE_ATOMIC(s8)*)flag, &o, 1, model, model);
}
#define lake_atomic_flag_clear_explicit(FLAG, MODEL) \
    lake_atomic_write_explicit((LAKE_ATOMIC(s8)*)FLAG, 0, MODEL)

LAKE_FORCE_INLINE bool _lake_atomic_msvc_is_lock_free_s8(LAKE_ATOMIC(s8) *obj) { return 1; }
LAKE_FORCE_INLINE bool _lake_atomic_msvc_is_lock_free_s16(LAKE_ATOMIC(s16) *obj) { return 1; }
LAKE_FORCE_INLINE bool _lake_atomic_msvc_is_lock_free_s32(LAKE_ATOMIC(s32) *obj) { return 1; }
LAKE_FORCE_INLINE bool _lake_atomic_msvc_is_lock_free_s64(LAKE_ATOMIC(s64) *obj) { return 1; }
LAKE_FORCE_INLINE bool _lake_atomic_msvc_is_lock_free_u8(LAKE_ATOMIC(u8) *obj) { return 1; }
LAKE_FORCE_INLINE bool _lake_atomic_msvc_is_lock_free_u16(LAKE_ATOMIC(u16) *obj) { return 1; }
LAKE_FORCE_INLINE bool _lake_atomic_msvc_is_lock_free_u32(LAKE_ATOMIC(u32) *obj) { return 1; }
LAKE_FORCE_INLINE bool _lake_atomic_msvc_is_lock_free_u64(LAKE_ATOMIC(u64) *obj) { return 1; }
LAKE_FORCE_INLINE bool _lake_atomic_msvc_is_lock_free_ptr(LAKE_ATOMIC(void *) *obj) { return 1; }
LAKE_FORCE_INLINE bool _lake_atomic_msvc_is_lock_free_unhandled(void *obj) { return 0; }

#define lake_atomic_is_lock_free(OBJ)                               \
    _Generic((OBJ),                                                 \
        LAKE_ATOMIC(s8)*: _lake_atomic_msvc_is_lock_free_s8,        \
        LAKE_ATOMIC(s16)*: _lake_atomic_msvc_is_lock_free_s16,      \
        LAKE_ATOMIC(s32)*: _lake_atomic_msvc_is_lock_free_s32,      \
        LAKE_ATOMIC(s64)*: _lake_atomic_msvc_is_lock_free_s64,      \
        LAKE_ATOMIC(u8)*: _lake_atomic_msvc_is_lock_free_u8,        \
        LAKE_ATOMIC(u16)*: _lake_atomic_msvc_is_lock_free_u16,      \
        LAKE_ATOMIC(u32)*: _lake_atomic_msvc_is_lock_free_u32,      \
        LAKE_ATOMIC(u64)*: _lake_atomic_msvc_is_lock_free_u64,      \
        LAKE_ATOMIC(void *)*: _lake_atomic_msvc_is_lock_free_ptr,   \
        default: _lake_atomic_msvc_is_lock_free_unhandled           \
    )(OBJ)
