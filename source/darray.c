#include <lake/data_structures/darray.h>
#include <lake/math/bits.h>

void lake_darray_init_w_dbg(lake_darray *da, s32 stride, s32 align, s32 n, char const *type)
{
    lake_dbg_assert(stride != 0, LAKE_INVALID_PARAMETERS, "%s", type);
    (void)type;

    n = lake_bits_next_pow2(n);
    if (n < 2) n = 2;

    da->size = 0;
    da->alloc = n;
    da->v = __lake_malloc(stride * n, align);
}

lake_darray lake_darray_copy(lake_darray const *da, s32 stride, s32 align)
{
    s32 new_alloc = stride * da->alloc;
    return (lake_darray){
        .v = new_alloc 
            ? lake_memcpy(__lake_malloc(new_alloc, align), da->v, new_alloc) 
            : nullptr,
        .size = da->size,
        .alloc = da->alloc,
    };
}

void lake_darray_reclaim(lake_darray *da, s32 stride, s32 align)
{
    s32 size = da->size;
    s32 new_alloc = stride * size;
    if (size < da->alloc) {
        if (size) {
            /* Don't use realloc as it will return the same size buffer when 
             * the new size is smaller than the existing size, which defeats 
             * the purpose of reclaim. */
            void *array = lake_memcpy(__lake_malloc(new_alloc, align), da->v, new_alloc);
            __lake_free(da->v);
            da->v = array;
            da->alloc = size;
        } else {
            lake_darray_fini(da);
        }
    }
}

void lake_darray_resize_w_dbg(lake_darray *da, s32 stride, s32 align, s32 n, char const *type)
{
    if (da->alloc == 0) {
        lake_darray_init_w_dbg(da, stride, align, n, type);
    } else if (da->alloc != n) {
        if (n < da->size)
            n = da->size;

        n = lake_bits_next_pow2(n);
        if (n < 2) n = 2;

        if (n != da->alloc) {
            da->v = __lake_realloc(da->v, stride * n, align);
            da->alloc = n;
        }
    }
}
