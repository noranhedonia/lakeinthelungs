#pragma once

/** @file lake/data_structures/arena_allocator.h 
 *  @brief TODO docs 
 */
#include <lake/bedrock/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct lake_arena_page {
    void                   *v;
    struct lake_arena_page *next;    
    s32                     offset;
    s32                     size;
} lake_arena_page;

typedef struct lake_arena_scratch {
    lake_arena_page        *tail;
    ssize                   offset;
} lake_arena_scratch;

typedef struct lake_arena_allocator {
    lake_arena_page        *head;
    lake_arena_page        *tail;
} lake_arena_allocator;

#ifdef __cplusplus
}
#endif /* __cplusplus */
