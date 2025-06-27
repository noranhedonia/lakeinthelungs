#pragma once

/** @file lake/data_structures/stack_allocator.h 
 *  @brief TODO docs 
 */
#include <lake/bedrock/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct lake_stack_page {
    void                       *v;
    struct lake_stack_page     *next;
    s16                         sp;
    u32                         idx;
} lake_stack_page;

typedef struct lake_stack_cursor {
    struct lake_stack_cursor   *prev;
    lake_stack_page            *stack;
    s16                         sp;
    bool                        is_free;
} lake_stack_cursor;

typedef struct lake_stack_allocator {
    lake_stack_page            *head;
    lake_stack_page            *tail_page;
    lake_stack_cursor          *tail_cursor;
} lake_stack_allocator;

#ifdef __cplusplus
}
#endif /* __cplusplus */
