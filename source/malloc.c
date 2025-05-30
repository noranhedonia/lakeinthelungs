#include <lake/bedrock/bedrock.h>

struct malloc_allocation_header {
    void *outer;    /**< Unaligned pointer returned by malloc(). */
    usize size;     /**< Original size from the requested allocation. */
};

void *__lake_malloc(
    usize size, 
    usize align)
{
    lake_dbg_assert(lake_is_pow2(align), LAKE_INVALID_PARAMETERS, nullptr);

    if (size == 0)
        return nullptr;

    void *outer = malloc(align + sizeof(struct malloc_allocation_header) + size);
    if (!outer)
        return nullptr;

    /* add enough space for the header, then round up to alignment */
    uptr inner = ((uptr)outer + sizeof(struct malloc_allocation_header) + align - 1) & ~(align - 1);

    struct malloc_allocation_header header;
    header.outer = outer;
    header.size = size;

    /* store the header just before inner */
    lake_memcpy((void *)(inner - sizeof(struct malloc_allocation_header)), &header, sizeof(struct malloc_allocation_header));
    return (void *)inner;
}

void *__lake_realloc(
    void *ptr, 
    usize size, 
    usize align)
{
    lake_dbg_assert(lake_is_pow2(align), LAKE_INVALID_PARAMETERS, nullptr);

    if (ptr == nullptr)
        return nullptr;

    if (size == 0) {
        __lake_free(ptr);
        return nullptr;
    }

    uptr inner = (uptr)ptr;

    /* header of the original allocation */
    struct malloc_allocation_header header;
    lake_memcpy(&header, (void *)(inner - sizeof(struct malloc_allocation_header)), sizeof(struct malloc_allocation_header));

    /* If we can be certain that realloc will return a correctly-aligned pointer (which typically 
     * means alignment <= alignof(double)) then it's most efficiently to simply use that.
     *
     * Otherwise, we have no choice but to allocate a fresh buffer and copy the data across.
     * We can't speculatively try a realloc and hope that it just shrinks the buffer and preserves 
     * alignment - the problem is that if realloc breaks the alignment, and we need to fall back 
     * to the fresh-buffer-and-copy method, but the fresh allocation fails, we will have already 
     * freed the original buffer (in realloc). We can only legally return NULL if we guarantee
     * the original buffer is still valid. */
    static const usize min_realloc_alignment = alignof(f64);

    if (align <= min_realloc_alignment) {
        void *new_outer = realloc(header.outer, align + sizeof(struct malloc_allocation_header) + size);
        if (!new_outer)
            return NULL;

        /* verify realloc returned the alignment we expected */
        lake_san_assert(((uptr)new_outer & (alignment - 1)) == 0, LAKE_ERROR_PANIC, nullptr);

        /* realloc already copied the inner contents, we just need to update the header */
        uptr new_inner = ((uptr)new_outer + sizeof(struct malloc_allocation_header) + align - 1) & ~(align - 1);

        /* double check */
        lake_san_assert(new_inner - inner == (uptr)new_outer - (uptr)header.outer, LAKE_ERROR_PANIC, nullptr);

        /* update the header */
        header.outer = new_outer;
        header.size = size;

        /* store the updated header */
        lake_memcpy((void *)(new_inner - sizeof(struct malloc_allocation_header)), &header, sizeof(struct malloc_allocation_header));

        return (void *)new_inner;
    } else {
        /* get a totally new aligned buffer */
        void *new_inner = __lake_malloc(size, align);
        if (!new_inner)
            return nullptr;
        
        /* copy the inner buffer */
        lake_memcpy(new_inner, (void *)inner, lake_min(size, header.size));

        /* release the original buffer */
        __lake_free(header.outer);
        return new_inner;
    }
}

void __lake_free(void *ptr)
{
    if (ptr == nullptr) return;

    uptr inner = (uptr)ptr;
    struct malloc_allocation_header header;

    lake_memcpy(&header, (void *)(inner - sizeof(struct malloc_allocation_header)), sizeof(struct malloc_allocation_header));
    free(header.outer);
}
