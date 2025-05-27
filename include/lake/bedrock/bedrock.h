#pragma once

#include <lake/bedrock/compiler.h>
#include <lake/bedrock/platforms.h>
#include <lake/bedrock/sanitize.h>
#include <lake/bedrock/arch.h>
#include <lake/bedrock/alignment.h>
#include <lake/bedrock/types.h>
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/complex.h>
#include <lake/bedrock/endian.h>
#include <lake/bedrock/magic.h>
#include <lake/bedrock/misc.h>
#include <lake/bedrock/time.h>
#include <lake/bedrock/simd.h>
#include <lake/bedrock/work.h>
#include <lake/bedrock/log.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Helper to declare the interface child object assembly procedure. */
#define PFN_LAKE_HANDLE_ASSEMBLY(T, arg, parent) \
    typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_##T##_##arg##_assembly)( \
        T##_##parent parent, T##_##arg##_assembly const *assembly, T##_##arg *out_##arg)
#define FN_LAKE_HANDLE_ASSEMBLY(backend, T, arg, parent) \
    LAKE_NODISCARD lake_result LAKECALL _##T##_##backend##_##arg##_assembly( \
        T##_##parent parent, T##_##arg##_assembly const *assembly, T##_##arg *out_##arg)

/** Helper to declare the interface child object destructor procedure. */
#define PFN_LAKE_HANDLE_DESTRUCTOR(T, arg) \
    PFN_LAKE_WORK(PFN_##T##_##arg##_destructor, T##_##arg arg)
#define FN_LAKE_HANDLE_DESTRUCTOR(backend, T, arg) \
    FN_LAKE_WORK(_##T##_##backend##_##arg##_destructor, T##_##arg arg)

/** Defines an opaque handle with private implementation. */
#define LAKE_DECL_HANDLE(T) \
    typedef struct T##_impl *T 

#define LAKE_DECL_HANDLE_HEADER(T, IMPL, PARENT)    \
    typedef struct T##_##IMPL##_header {            \
        union {                                     \
            struct T##_##PARENT##_impl   *impl;     \
            struct T##_##PARENT##_header *header;   \
        } PARENT;                                   \
        lake_refcnt             refcnt;             \
        T##_##IMPL##_assembly   assembly;           \
    } T##_##IMPL##_header

/** Systems can use an atomic counter for references to itself. A reference count of 0
 *  or less means that the system can be safely destroyed, as it is no longer in use. */
typedef atomic_s32 lake_refcnt;

/** An interface header for other systems to inherit. Used to abstract over implementations. */
LAKE_CACHELINE_ALIGNMENT
typedef struct lake_interface_header {
    /** Tracks reference count to this interface. */
    lake_refcnt         refcnt;
    /** Will be called to destroy the interface, the work argument is self. */
    PFN_lake_work       destructor;
    /** An unique name of the interface implementation, can be colored. */
    lake_small_string   name;
} lake_interface_header;

/* TODO framework */
LAKE_NONNULL_ALL
LAKEAPI s32 LAKECALL
lake_in_the_lungs(PFN_lake_work framework, void *userdata);

#ifdef __cplusplus
}
#endif /* __cplusplus */
