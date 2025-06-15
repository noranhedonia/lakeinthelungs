#pragma once

/** @file lake/data_structures/strbuf.h 
 *  @brief A string buffer.
 *
 *  These are meant to be used with the usual C string and memory APIs.
 *  Given that the length of the buffer is known, it's often better to 
 *  use the standard mem* functions than the str* ones (e.g. memchr vs strchr).
 *  A strbuf is NIL terminated for convenience, but no function in this API 
 *  actually relies on the string being free of NULs.
 */
#include <lake/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** The string buffer structure. */
typedef struct lake_strbuf {
    char *v;        /**< A byte array of atleast `len + 1` bytes allocated. */
    s32   len;      /**< Used to determine the current length of the string. */
    s32   alloc;    /**< A "private" member describing the capacity of the buffer. */
} lake_strbuf;

LAKEAPI void LAKECALL 
lake_strbuf_appendstrn(
        lake_strbuf    *buf, 
        char const     *str, 
        s32             n);

#ifdef __cplusplus
}
#endif /* __cplusplus */
