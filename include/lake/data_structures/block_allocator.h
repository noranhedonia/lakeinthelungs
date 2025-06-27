#pragma once

/** @file lake/data_structures/block_allocator.h 
 *  @brief TODO docs 
 */
#include <lake/bedrock/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct lake_block_page {
    u8                         *v;
    struct lake_block_page     *next;
} lake_block_page;

typedef struct lake_block_chunk {
    struct lake_block_chunk    *next;
} lake_block_chunk;

typedef struct lake_block_allocator {
    s32                         data_size;
    s32                         chunk_size;
    s32                         chunks_per_block;
    s32                         block_size;
    lake_block_chunk           *head;
    lake_block_page            *block_head;
} lake_block_allocator;

#ifdef __cplusplus
}
#endif /* __cplusplus */
