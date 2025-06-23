#include <lake/data_structures/deque.h>

lake_result lake_deque_resize_w_dbg(
    lake_deque *deq,
    s32         stride,
    s32         align,
    s32         n,
    char const *type)
{
    void *v;
    lake_dbg_assert(stride != 0, LAKE_INVALID_PARAMETERS, "%s", type);
    lake_dbg_assert(n > 0 && n >= deq->len, LAKE_INVALID_PARAMETERS, "%s", type);
    (void)type;

    if (!(v = __lake_malloc(stride * n, align))) 
        return LAKE_ERROR_OUT_OF_HOST_MEMORY;

    if (deq->len) {
        s32 part1 = deq->head + deq->len <= deq->cap ? deq->len : deq->cap - deq->head;
        s32 part2 = deq->len - part1;
        lake_memcpy(v, lake_elem(deq->v, stride, deq->head), stride * part1);
        if (part2) 
            lake_memcpy(lake_elem(v, stride, part1), deq->v, stride * part2);
    }
    if (deq->cap) 
        __lake_free(deq->v);

    deq->v = v;
    deq->head = 0;
    deq->tail = deq->len;
    deq->cap = n;
    return LAKE_SUCCESS;
}

s32 lake_deque_op_w_dbg(
    lake_deque     *deq, 
    s32             stride, 
    s32             align, 
    lake_deque_op   op,
    char const     *type)
{
    s32 idx = -1;
#define RESIZE_DEQUE(n) \
    (lake_deque_resize_w_dbg(deq, stride, align, (n), type) != LAKE_SUCCESS)

    switch (op) {
        case lake_deque_op_push:
        case lake_deque_op_unshift:
            if (deq->len == deq->cap && RESIZE_DEQUE(deq->cap == 0 ? deq->min : deq->cap * 2))
                return -1;
            break;
        case lake_deque_op_pop:
        case lake_deque_op_shift:
            if (deq->cap > deq->min) {
                if (deq->shrink == lake_deque_shrink_if_empty && deq->len == 1 && RESIZE_DEQUE(deq->min))
                    return -1;
                if (deq->shrink == lake_deque_shrink_at_one_fifth && (deq->len)*5 <= deq->cap && RESIZE_DEQUE(deq->cap >> 1))
                    return -1;
            }
            if (deq->len == 0)
                return -1;
    }
    lake_dbg_assert(deq->cap > 0, LAKE_PANIC, nullptr);
#undef RESIZE_DEQUE

    switch(op) {
        case lake_deque_op_push:
            idx = deq->tail++;
            deq->tail %= deq->cap;
            deq->len++;
            break;
        case lake_deque_op_shift:
            idx = deq->head++;
            deq->head %= deq->cap;
            deq->len--;
            break;
        case lake_deque_op_pop:
            deq->tail = (deq->tail == 0 ? deq->cap : deq->tail) - 1;
            idx = deq->tail;
            deq->len--;
            break;
        case lake_deque_op_unshift:
            deq->head = (deq->head == 0 ? deq->cap : deq->head) - 1;
            idx = deq->head;
            deq->len++;
            break;
    };
    return idx;
}
