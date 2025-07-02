#pragma once

/** @file lake/data_structures/balloc.h 
 *  @brief Block allocator.
 *
 *  TODO docs 
 */
#include <lake/bedrock/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct lake_balloc_page {
    u8                         *v;
    struct lake_balloc_page     *next;
} lake_balloc_page;

typedef struct lake_balloc_chunk {
    struct lake_balloc_chunk    *next;
} lake_balloc_chunk;

typedef struct lake_balloc {
    s32                         data_size;
    s32                         chunk_size;
    s32                         chunks_per_block;
    s32                         block_size;
    lake_balloc_chunk          *head;
    lake_balloc_page           *block_head;
} lake_balloc;

#ifdef __cplusplus
}
#endif /* __cplusplus */
