#include <lake/data_structures/arena.h>
#include <lake/data_structures/balloc.h>
#include <lake/data_structures/bitset.h>
#include <lake/data_structures/dagraph.h>
#include <lake/data_structures/darray.h>
#include <lake/data_structures/deque.h>
#include <lake/data_structures/hashmap.h>
#include <lake/data_structures/map.h>
#include <lake/data_structures/mpmc.h>
#include <lake/data_structures/sparse.h>
#include <lake/data_structures/stack.h>
#include <lake/data_structures/strbuf.h>
#include <lake/data_structures/switch.h>

void *lake_arena_w_drifter(lake_arena *a, s32 size, s32 align)
{
    return lake_arena_allocate(a, size, align, lake_drifter);
}

void *lake_arena_w_machina(lake_arena *a, s32 size, s32 align)
{
    return lake_arena_allocate(a, size, align, lake_machina);
}

bool lake_mpmc_rotate(
    lake_mpmc          *mpmc,
    atomic_ssize       *in_or_out,
    s32 const           stride,
    s32 const           pos_delta,
    lake_mpmc_result   *out_result) 
{
    atomic_ssize *sequence;
    ssize pos = lake_atomic_read_explicit(in_or_out, lake_memory_model_relaxed);

    for (;;) {
        sequence = (atomic_ssize *)lake_elem(mpmc->buffer, stride, pos & mpmc->buffer_mask);
        ssize seq = lake_atomic_read_explicit(sequence, lake_memory_model_acquire);
        sptr diff = (sptr)seq - (sptr)(pos + pos_delta);

        if (diff == 0) {
            if (lake_atomic_compare_exchange_weak_explicit(in_or_out, &pos , pos + 1,
                    lake_memory_model_relaxed, lake_memory_model_relaxed)) 
            {
                *out_result = (lake_mpmc_result){ .node = (void *)sequence, .pos = pos };
                return true;
            }
        } else if (diff < 0) {
            /* it's empty */
            *out_result = (lake_mpmc_result){ .node = nullptr, .pos = 0 };
            return false;
        } else {
            pos = lake_atomic_read_explicit(in_or_out, lake_memory_model_relaxed);
        }
    }
    LAKE_UNREACHABLE;
}

char lake_strbuf_slopbuf[] = {'\0'};

void lake_strbuf_appendstrn(
        lake_strbuf    *buf, 
        char const     *str, 
        s32             n)
{
    lake_dbg_assert(buf->v && buf->alloc, LAKE_ERROR_MEMORY_MAP_FAILED, nullptr);

    n = lake_min(n, buf->alloc - (buf->len + n));
    if (n <= 0) return;

    lake_memcpy(&buf->v[buf->len], str, n);
    buf->len += n;
}
