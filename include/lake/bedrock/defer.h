#pragma once

/** @file lake/bedrock/defer.h 
 *  @brief Implements the defer statement using computed goto.
 *
 *  Until the real thing is available with new C2Y standards, I'll be using this
 *  custom solution. It was somewhat inspired by this blog post;
 *  https://thephd.dev/c2y-the-defer-technical-specification-its-time-go-go-go
 *
 *  Some important notes:
 *  - The parameter to lake_defer() can be arbitrary in-scope code. It's defined 
 *    as a variable argument list, so the preprocessor shouldn't be angry.
 *  - Defer blocks only run if they are dynamically reached.
 *  - Any such block is only ever run a single time.
 *  - Defer blocks run in reverse order in which they were reached.
 *  - You MUST NOT jump out of whatever code is put inside the defer statement,
 *    there is no guard against it. 
 *  - The return statement is run AFTER all reached defer blocks run. This also 
 *    means, if any computation is done directly on the return, it will run AFTER 
 *    the defer blocks were evaluated.
 *
 *  Each defer block statically declares its own stack-allocated record in a linked 
 *  list. When a defer block is entered, it first checks to see if it's been run yet 
 *  (done via a sentinal value set in the stack entry). If it hasn't been entered, 
 *  then it pushes the location of the code to run during cleanup onto the stack. 
 *  The rest of the defer block is skipped. The deferred return (and longjmp) 
 *  statements ensure the cleanup happens before actually executing return. 
 *
 *  The implementation makes use of computed goto statements. They generally work
 *  in GCC and Clang (not sure about MSVC). Goto in C targets a fixed, static label 
 *  that will translate into a jump instruction to a fixed offset known at compile 
 *  time, which will be incredibly cheap. Here though, while we create jump labels
 *  associated with each defer statement statically, we only want to jump to things
 *  if they're triggered. So which labels we want to jump to (and in what order) is 
 *  dynamic. The computed goto allows us to handle that by giving two primitives:
 *
 *  - The ability to get the address of any in-scope label that we want to use as 
 *    a jump target. This is done with the && operator. The type of the label is 
 *    simply void *, though one could typedef it for clarity...
 *
 *  - The ability to give goto a memory location from which to dynamically read 
 *    the jump target. This is done by using the C dereference operator (*), 
 *    effectively telegraphing the argument is not the jump target, but where 
 *    to LOOK for the jump target.
 *
 *  The longjmp call allows for `goto`-like functionality that can cross function 
 *  boundaries, and essentially works by saving register state (at the point of a 
 *  setjmp), and then restoring it all with the longjmp, which is significant work 
 *  compared to a few gotos and a small single digit number of stack accesses per 
 *  frame. This approach with computed goto is much more lightweight. The longjmp 
 *  is actually similar to what we're doing with make_fcontext and jump_fcontext.
 *
 *  Of course, we do wrap longjmp, since it's commonly used as the basis of more 
 *  heavy-weight exception handling mechanisms. The biggest challenge with such 
 *  mechanisms is often doing the cleanup while unwinding the stack. Wrapping 
 *  longjmp doesn't directly help a lot there, as it only would run defer blocks 
 *  when raising an exception. However, it's easy to trigger `defer` calls at the 
 *  point where setjmp gets back to execution after a jump completes. 
 *
 *  While the implementation is conceptually simple and doesn't take much code,
 *  it does require some C wizardry to understand:
 *
 *  1. C's token pasting operator (## in magic glue macro) allows us to create 
 *     variables and jump labels on the fly.
 *
 *  2. We can make labels and variables unique per-statement by pasting together 
 *     a prefix with the value of the LINE macro (done with an indirect macro here).
 *
 *  3. Wrapping defer code in an `if (false) {}` block allows us to skip it on 
 *     the first run. But if we define a label inside the block, then push that 
 *     label onto the stack, it can be popped off the block and jumped to.
 *
 *  4. After the user's defer code (but still inside the `if (false) {}` block), 
 *     we need to add code to check if there's another deferred block we have 
 *     reached, that we need to run. We always look inside our own private 
 *     table of records to see the successor, if any.
 *
 *  5. When there are no more defer blocks to run, we need to jump to the actual 
 *     return statement (or longjmp). A function can have many return statements.
 *     To address that, there's a single variable (added via `lake_defer_begin()`)
 *     that gets the jump target to complete the exit. So when the stack empties,
 *     we jump wherever that is really. We use the same token-pasting approach 
 *     to create the label, and a computed goto to jump to it.
 *
 *  6. The guard determines whether a defer block has been added to the record 
 *     table or not, it helps prevent blocks from being run multiple times on 
 *     exit. Since the compiler does generate code to zero out stack-allocated 
 *     memory, we have to get a bit creative.
 *
 *     Here we assume the stack memory is random garbage, and use an arbitrary 
 *     guard value to indicate that a block has been run. When doing this, we 
 *     need to be sure to remove the guard (by zeroing it out) so that we don't 
 *     miss a deferred block in subsequent stack frames.
 *
 *     We could take an alternate approach, and statically initialize a boolean.
 *     We still would need to clear it after running our defer blocks. That keeps
 *     it off the stack, but leaves us hosed if we somehow do jump out of the block.
 *
 *  I'm looking forward for compiler changes to bring in support for defer in C ;3
 *  Or I will make my own language if I will feel bored, who knows...
 */
#include <lake/bedrock/log.h>
#include <lake/bedrock/magic.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct lake_deferred_record {
    void   *next_target;
    int64_t guard;
} lake_deferred_record;

#define LAKE_DEFER_GUARD_INIT ((int64_t)0xdefe11defe11defeLL)

#define lake_defer_begin()                      \
    lake_deferred_record __deferred_table = {   \
        NULL,                                   \
        LAKE_DEFER_GUARD_INIT,                  \
    };                                          \
    void *__deferred_return_label = nullptr

#define lake_defer_label(x) LAKE_MAGIC_GLUE2(__defer_block_, x)

#define lake_defer_node(x) LAKE_MAGIC_GLUE2(__defer_node_, x)

/* The unnecessary extra block after the label is to prevent
 * clang-format from wrapping oddly. */
#define lake_defer(...)                                                             \
    lake_deferred_record lake_defer_node(__LINE__) = {0ULL};                        \
    if (lake_likely(lake_defer_node(__LINE__).guard != LAKE_DEFER_GUARD_INIT)) {    \
        lake_defer_node(__LINE__).guard       = LAKE_DEFER_GUARD_INIT;              \
        lake_defer_node(__LINE__).next_target = __deferred_table.next_target;       \
        __deferred_table.next_target          = && lake_defer_label(__LINE__);      \
    }                                                                               \
    if (false) {                                                                    \
        lake_defer_label(__LINE__) :                                                \
        {                                                                           \
            lake_defer_node(__LINE__).guard = 0ULL;                                 \
            __VA_ARGS__                                                             \
            if (!lake_defer_node(__LINE__).next_target) {                           \
                goto *(__deferred_return_label);                                    \
            }                                                                       \
        }                                                                           \
        goto *(lake_defer_node(__LINE__).next_target);                              \
    }

#define lake_defer_fn_exit()                    \
    if (__deferred_table.next_target) {         \
        goto *(__deferred_table.next_target);   \
    }

#define lake_defer_return                                   \
    __deferred_return_label = &&lake_defer_label(__LINE__); \
    lake_defer_fn_exit();                                   \
    lake_defer_label(__LINE__) : return

#define lake_defer_return_if_status(status) \
    if (status != LAKE_SUCCESS) { lake_defer_return status; }

#define lake_defer_longjmp(jump_env, jump_passed_state)     \
    __deferred_return_label = &&lake_defer_label(__LINE__); \
    lake_defer_fn_exit();                                   \
    lake_defer_label(__LINE__) : longjmp(jump_env, jump_passed_state)

#ifdef __cplusplus
}
#endif /* __cplusplus */
