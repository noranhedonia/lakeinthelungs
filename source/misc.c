#include <lake/bedrock/log.h>
#include <lake/bedrock/misc.h>
#include <lake/bedrock/endian.h>

#if defined(LAKE_LITTLE_ENDIAN)
#define LS >>
#define RS <<
#else
#define LS <<
#define RS >>
#endif

#define ONES ((usize) - 1 /UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2 + 1))
#define HASZERO(x) (((x) - ONES) & ~(x) & HIGHS)

#include <stdlib.h>

void *lake_memset(
    void *restrict dst, 
    s32            c, 
    usize          n)
{
    u8 *s = dst;
    usize k;

    /* Fill head and tail with minimal branching. Each conditional 
     * ensures that all the subsequently used offsets are well-defined 
     * and in the dest region. */

    if (!n) return dst;
    s[0] = c;
    s[n-1] = c;
    if (n <= 2) return dst;
    s[1] = c;
    s[2] = c;
    s[n-2] = c;
    s[n-3] = c;
    if (n <= 6) return dst;
    s[3] = c;
    s[n-4] = c;
    if (n <= 8) return dst;

    /* Advance pointer to align it at a 4-byte boundary, and truncate n to 
     * a multiple of 4. The previous code already took care of any head/tail 
     * that get cut off by the alignment. */

    k = -(uptr)s & 3;
    s += k;
    n -= k;
    n &= -4;

#if LAKE_HAS_MAY_ALIAS
    alias_u32 c32 = ((alias_u32) - 1) / 255 * (u8)c;

    /* In preparation to copy 32 bits at a time, aligned on an 8-byte boundary,
     * fill head/tail up to 28 bytes each. As in the initial byte-based head/tail 
     * fill, each conditional below ensures that the subsequent offsets are valid. */

    *(alias_u32 *)(s + 0) = c32;
    *(alias_u32 *)(s + n - 4) = c32;
    if (n <= 8) return dst;
    *(alias_u32 *)(s + 4) = c32;
    *(alias_u32 *)(s + 8) = c32;
    *(alias_u32 *)(s + n - 12) = c32;
    *(alias_u32 *)(s + n - 8) = c32;
    if (n <= 24) return dst;
    *(alias_u32 *)(s + 12) = c32;
    *(alias_u32 *)(s + 16) = c32;
    *(alias_u32 *)(s + 20) = c32;
    *(alias_u32 *)(s + 24) = c32;
    *(alias_u32 *)(s + n - 28) = c32;
    *(alias_u32 *)(s + n - 24) = c32;
    *(alias_u32 *)(s + n - 20) = c32;
    *(alias_u32 *)(s + n - 16) = c32;

    /* Align to a multiple of 8, so we can fill 64 bits at a time,
     * and avoid writing the same bytes twice as mush as is practical 
     * without introducing additional branching. */

    k = 24 + ((uptr)s & 4);
    s += k;
    n -= k;

    /* If this loop is reached, 28 tail bytes have already been filled,
     * so any remainder when n drops below 32 can be safely ignored. */

    alias_u64 c64 = c32 | ((alias_u64)c32 << 32);
    for (; n >= 32; n -= 32, s += 32) {
        *(alias_u64 *)(s + 0) = c64;
        *(alias_u64 *)(s + 8) = c64;
        *(alias_u64 *)(s + 16) = c64;
        *(alias_u64 *)(s + 24) = c64;
    }
#else
    /* Pure C fallback with no aliasing violations. */
    for (; n; n--, s++) *s = c;
#endif /* LAKE_HAS_MAY_ALIAS */
    return dst;
}

void *lake_memcpy(
    void       *restrict dst, 
    void const *restrict src, 
    usize                n)
{
    u8       *d = dst;
    u8 const *s = src;

#if LAKE_HAS_MAY_ALIAS
    alias_u32 w, x;

	for (; (uptr)s % 4 && n; n--) *d++ = *s++;

	if ((uptr)d % 4 == 0) {
		for (; n >= 16; s += 16, d += 16, n -= 16) {
			*(alias_u32 *)(d+0) = *(alias_u32 *)(s+0);
			*(alias_u32 *)(d+4) = *(alias_u32 *)(s+4);
			*(alias_u32 *)(d+8) = *(alias_u32 *)(s+8);
			*(alias_u32 *)(d+12) = *(alias_u32 *)(s+12);
		}
		if (n & 8) {
			*(alias_u32 *)(d+0) = *(alias_u32 *)(s+0);
			*(alias_u32 *)(d+4) = *(alias_u32 *)(s+4);
			d += 8; s += 8;
		}
		if (n & 4) {
			*(alias_u32 *)(d+0) = *(alias_u32 *)(s+0);
			d += 4; s += 4;
		}
		if (n & 2) {
			*d++ = *s++; *d++ = *s++;
		}
		if (n & 1) {
			*d = *s;
		}
		return dst;
	}

	if (n >= 32) switch ((uptr)d % 4) {
	case 1:
		w = *(alias_u32 *)s;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		n -= 3;
		for (; n >= 17; s += 16, d += 16, n -= 16) {
			x = *(alias_u32 *)(s+1);
			*(alias_u32 *)(d+0) = (w LS 24) | (x RS 8);
			w = *(alias_u32 *)(s+5);
			*(alias_u32 *)(d+4) = (x LS 24) | (w RS 8);
			x = *(alias_u32 *)(s+9);
			*(alias_u32 *)(d+8) = (w LS 24) | (x RS 8);
			w = *(alias_u32 *)(s+13);
			*(alias_u32 *)(d+12) = (x LS 24) | (w RS 8);
		}
		break;
	case 2:
		w = *(alias_u32 *)s;
		*d++ = *s++;
		*d++ = *s++;
		n -= 2;
		for (; n >= 18; s += 16, d += 16, n -=16) {
			x = *(alias_u32 *)(s+2);
			*(alias_u32 *)(d+0) = (w LS 16) | (x RS 16);
			w = *(alias_u32 *)(s+6);
			*(alias_u32 *)(d+4) = (x LS 16) | (w RS 16);
			x = *(alias_u32 *)(s+10);
			*(alias_u32 *)(d+8) = (w LS 16) | (x RS 16);
			w = *(alias_u32 *)(s+14);
			*(alias_u32 *)(d+12) = (x LS 16) | (w RS 16);
		}
		break;
	case 3:
		w = *(alias_u32 *)s;
		*d++ = *s++;
		n -= 1;
		for (; n >= 19; s += 16, d += 16, n -= 16) {
			x = *(alias_u32 *)(s+3);
			*(alias_u32 *)(d+0) = (w LS 8) | (x RS 24);
			w = *(alias_u32 *)(s+7);
			*(alias_u32 *)(d+4) = (x LS 8) | (w RS 24);
			x = *(alias_u32 *)(s+11);
			*(alias_u32 *)(d+8) = (w LS 8) | (x RS 24);
			w = *(alias_u32 *)(s+15);
			*(alias_u32 *)(d+12) = (x LS 8) | (w RS 24);
		}
		break;
	}
	if (n & 16) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (n & 8) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (n & 4) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (n & 2) {
		*d++ = *s++; *d++ = *s++;
	}
	if (n & 1) {
		*d = *s;
	}
	return dst;
#else
    /* Pure C fallback with no aliasing violations. */
    for (; n; n--) *d++ = *s++;
#endif /* LAKE_HAS_MAY_ALIAS */
    return dst;
}

s32 lake_memcmp(
    void const *vl, 
    void const *vr, 
    usize       n)
{
    u8 const *l = vl, *r = vr;
    for (; n && *l == *r; n--, l++, r++);
    return n ? *l-*r : 0;
}

void *lake_memrchr(
    void const *restrict mem, 
    s32                  c, 
    usize                n)
{
    u8 const *raw = mem;
    c = (u8)c;
    while (n--) if (raw[n] == c) return (void *)(raw + n);
    return 0;
}

usize lake_strlen(char const *str)
{
    char const *a = str;
#if LAKE_HAS_MAY_ALIAS
    alias_word const *w;
    for (; (uptr)str % sizeof(usize); str++) 
        if (!*str) return str - a;
    for (w = (void const *)str; !((*w) - ((usize) - 1/UCHAR_MAX) * (UCHAR_MAX/2 + 1)); w++);
    str = (void const *)w;
#endif /* LAKE_HAS_MAY_ALIAS */
    for (; *str; str++);
    return str - a;
}

s32 lake_strncmp(
    char const *vl, 
    char const *vr, 
    usize       n)
{
    u8 const *l = (void const *)vl, *r = (void const *)vr;
    if (!n--) return 0;
    for (; *l && *r && n && *l == *r; n--, l++, r++);
    return *l - *r;
}

char *lake_strncpy(
    char       *restrict dst, 
    char const *restrict str, 
    usize                n)
{
    usize       *wd;
    usize const *ws;

    if (((uptr)str & (sizeof(usize)-1)) == ((uptr)dst & (sizeof(usize)-1))) {
        for (; ((uptr)str & (sizeof(usize)-1)) && n && (*dst = *str); n--, str++, dst++);
        if (!n || !*str) goto tail;

        wd = (void *)dst;
        ws = (void const *)str;
        for (; n >= sizeof(usize) && !HASZERO(*ws); n -= sizeof(usize), ws++, wd++) *wd = *ws;
        dst = (void *)wd;
        str = (void const *)ws;
    }
    for (; n && (*dst = *str); n--, str++, dst++);
tail:
    lake_memset(dst, 0, n);
    return dst;
}

char *lake_strchrnul(
    char const *str, 
    s32         c)
{
    usize *w, k;
    c = (u8)c;
    if (!c) return (char *)str + lake_strlen(str);

    for (; (uptr)str & (sizeof(usize)-1); str++)
        if (!*str || *(u8 *)str == c) return (char *)str;
    k = ONES * c;

    for (w = (void *)str; !HASZERO(*w) && !HASZERO(*w ^ k); w++);
    for (str = (void *)w; *str && *(u8 *)str != c; str++);
    return (char *)str;
}

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
    uptr inner = (uptr)ptr;
    struct malloc_allocation_header header;

    lake_memcpy(&header, (void *)(inner - sizeof(struct malloc_allocation_header)), sizeof(struct malloc_allocation_header));
    free(header.outer);
}
