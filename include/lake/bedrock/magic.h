#pragma once

/** @file lake/bedrock/magic.h 
 *  @brief Abuse of the C preprocessor.
 *
 *  This contains a bunch of fancy macro techniques such as preprocessor-time evaluated 
 *  conditionals and (quasi) recursion, and iteration.
 *
 *  Based on these articles:
 *  - http://jhnet.co.uk/articles/cpp_magic
 *  - https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms
 *  and code from the Boost C++ library.
 */

/** Expands to nothing. */
#define LAKE_MAGIC_NOTHING()

/** Convert arguments to a string literal */
#define _LAKE_MAGIC_STRINGIFY(...)  #__VA_ARGS__
#define LAKE_MAGIC_STRINGIFY(...)   _LAKE_MAGIC_STRINGIFY(__VA_ARGS__)

/** Glue arguments together. Expends to the expansion of @a_ followed immediately
 *  (combining tokens) by the expansion of @b_. */
#define _LAKE_MAGIC_GLUE2(a_, b_)   a_##b_
#define LAKE_MAGIC_GLUE2(a_, b_)    _LAKE_MAGIC_GLUE2(a_, b_)

/** Return first argument. Expands to the expansion of @a_ */
#define LAKE_MAGIC_1ST(a_, ...)     a_

/** Return second argument. Expands to the expansion ob @b_ */
#define LAKE_MAGIC_2ND(a_, b_, ...) b_

/** Is argument zero. Expands to one if @a is "0", otherwise expands to zero. */
#define _LAKE_MAGIC_ISPROBE(...)    LAKE_MAGIC_2ND(__VA_ARGS__, 0)
#define _LAKE_MAGIC_PROBE()         $, 1
#define _LAKE_MAGIC_ISZERO_0        _LAKE_MAGIC_PROBE()
#define LAKE_MAGIC_ISZERO(a_)       _LAKE_MAGIC_ISPROBE(LAKE_MAGIC_GLUE2(_LAKE_MAGIC_ISZERO_), a_)

/** Is argument non-zero. Expands to zero if @a is "0", otherwise expands to one. */
#define LAKE_MAGIC_NONZERO(a_)      LAKE_MAGIC_ISZERO(LAKE_MAGIC_ISZERO(a_))

/** Does the macro have any arguments. */
#define _LAKE_MAGIC_EOA()           0
#define LAKE_MAGIC_NONEMPTY(...)    LAKE_MAGIC_NONZERO(LAKE_MAGIC_1ST(_LAKE_MAGIC_EOA __VA_ARGS__)())

/** Preprocessor conditional. */
#define _LAKE_MAGIC_IF_0(...)       _LAKE_MAGIC_IF_0_ELSE
#define _LAKE_MAGIC_IF_1(...)       __VA_ARGS__ _LAKE_MAGIC_IF_1_ELSE
#define _LAKE_MAGIC_IF_0_ELSE(...)  __VA_ARGS__
#define _LAKE_MAGIC_IF_1_ELSE(...)
#define _LAKE_MAGIC_IF_ELSE(cond_)  LAKE_MAGIC_GLUE2(_LAKE_MAGIC_IF_, cond_)
#define LAKE_MAGIC_IF_ELSE(cond_)   _LAKE_MAGIC_IF_ELSE(LAKE_MAGIC_NONZERO(cond_))

/** Force multiple expansion passes. Arguments are expanded repeatedly (up to 1024 times)
 *  even when the C preprocessor would stop expanding. */
#define LAKE_MAGIC_EVAL1(...)       __VA_ARGS__
#define LAKE_MAGIC_EVAL2(...)       LAKE_MAGIC_EVAL1(LAKE_MAGIC_EVAL1(__VA_ARGS__))
#define LAKE_MAGIC_EVAL4(...)       LAKE_MAGIC_EVAL2(LAKE_MAGIC_EVAL2(__VA_ARGS__))
#define LAKE_MAGIC_EVAL8(...)       LAKE_MAGIC_EVAL4(LAKE_MAGIC_EVAL4(__VA_ARGS__))
#define LAKE_MAGIC_EVAL16(...)      LAKE_MAGIC_EVAL8(LAKE_MAGIC_EVAL8(__VA_ARGS__))
#define LAKE_MAGIC_EVAL32(...)      LAKE_MAGIC_EVAL16(LAKE_MAGIC_EVAL16(__VA_ARGS__))
#define LAKE_MAGIC_EVAL64(...)      LAKE_MAGIC_EVAL32(LAKE_MAGIC_EVAL32(__VA_ARGS__))
#define LAKE_MAGIC_EVAL128(...)     LAKE_MAGIC_EVAL64(LAKE_MAGIC_EVAL64(__VA_ARGS__))
#define LAKE_MAGIC_EVAL256(...)     LAKE_MAGIC_EVAL128(LAKE_MAGIC_EVAL128(__VA_ARGS__))
#define LAKE_MAGIC_EVAL512(...)     LAKE_MAGIC_EVAL256(LAKE_MAGIC_EVAL256(__VA_ARGS__))
#define LAKE_MAGIC_EVAL1024(...)    LAKE_MAGIC_EVAL512(LAKE_MAGIC_EVAL512(__VA_ARGS__))
#define LAKE_MAGIC_EVAL(...)        LAKE_MAGIC_EVAL1024(__VA_ARGS__)

/** Defer expansion. */
#define LAKE_MAGIC_DEFER1(a_)       a_ LAKE_MAGIC_NOTHING()
#define LAKE_MAGIC_DEFER2(a_)       a_ LAKE_MAGIC_NOTHING LAKE_MAGIC_NOTHING()()

/** Iterate another macro across arguments. Expands to the expansion of:
 *  @m(@a1), @m(@a2), ..., @m(@an). */
#define _LAKE_MAGIC_MAP_()          _LAKE_MAGIC_MAP
#define _LAKE_MAGIC_MAP(m_, a_, ...) \
    m_(a_) LAKE_MAGIC_IF_ELSE(LAKE_MAGIC_NONEMPTY(__VA_ARGS__)) \
        (LAKE_MAGIC_DEFER2(_LAKE_MAGIC_MAP_)()(m_, __VA_ARGS__))()
#define LAKE_MAGIC_MAP(m_, ...) \
    LAKE_MAGIC_IF_ELSE(LAKE_MAGIC_NONEMPTY(__VA_ARGS__)) \
        (LAKE_MAGIC_EVAL(_LAKE_MAGIC_MAP(m_, __VA_ARGS__)))()

/** Iterate another macro across pairs of arguments. Expands to the expansion of:
 *  @m(@a1, @b1), @m(@a1, @b2), ..., @m(@an, @bn). */
#define _LAKE_MAGIC_2MAP_()         _LAKE_MAGIC_2MAP
#define _LAKE_MAGIC_2MAP(m_, a_, b_, ...) \
    m_(a_, b_) LAKE_MAGIC_IF_ELSE(LAKE_MAGIC_NONEMPTY(__VA_ARGS__)) \
        (, LAKE_MAGIC_DEFER2(_LAKE_MAGIC_2MAP_)()(m_, __VA_ARGS__))()
#define LAKE_MAGIC_2MAP(m_, ...) \
    LAKE_MAGIC_IF_ELSE(LAKE_MAGIC_NONEMPTY(__VA_ARGS__)) \
        (LAKE_MAGIC_EVAL(_LAKE_MAGIC_2MAP(m_, __VA_ARGS__)))()

/** Separate arguments with given delimiter. Expands to the expansion of:
 *  @a1 @d @a2 @d ... @d @an */
#define _LAKE_MAGIC_JOIN_()         _LAKE_MAGIC_JOIN
#define _LAKE_MAGIC_JOIN(d_, a_, ...) \
    a_ LAKE_MAGIC_IF_ELSE(LAKE_MAGIC_NONEMPTY(__VA_ARGS__)) \
        (d_ LAKE_MAGIC_DEFER2(_LAKE_MAGIC_JOIN_)()(d_, __VA_ARGS__))()
#define LAKE_MAGIC_JOIN(d_, ...) \
    LAKE_MAGIC_IF_ELSE(LAKE_MAGIC_NONEMPTY(__VA_ARGS__)) \
        (LAKE_MAGIC_EVAL(_LAKE_MAGIC_JOIN(d_, __VA_ARGS__)))()
