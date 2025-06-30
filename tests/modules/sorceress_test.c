#include "../test_framework.h"

/* XXX we can't test the framework bedrock main from here, but it's fine as the game application somewhat 
 * does that aleady. Instead we can test for movements and other API calls that sorceress defines. */
static struct test_case_details g_tests_todo[] = {
    //IMPL_TEST_CASE(Volta_todo, todo),
};

#define IMPL_SORCERESS_TEST_SUITE(VAR) \
    FN_TEST_SUITE_INIT(Sorceress_##VAR) \
    { \
        *out = (struct test_suite_details){ \
            .count = lake_arraysize(g_tests_##VAR), \
            .tests = g_tests_##VAR, \
            .userdata = nullptr, \
        }; \
        (void)bedrock; \
    } \
    FN_TEST_SUITE_FINI(Sorceress_##VAR) { (void)userdata; }

IMPL_SORCERESS_TEST_SUITE(todo)
