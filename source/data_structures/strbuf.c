#include <lake/data_structures/strbuf.h>

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
