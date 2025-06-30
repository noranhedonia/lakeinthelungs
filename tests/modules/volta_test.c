#include "../test_framework.h"

static struct test_case_details g_tests_todo[] = {
    //IMPL_TEST_CASE(Volta_todo, todo),
};

#define IMPL_VOLTA_TEST_SUITE(VAR) \
    FN_TEST_SUITE_INIT(Volta_##VAR) \
    { \
        *out = (struct test_suite_details){ \
            .count = lake_arraysize(g_tests_##VAR), \
            .tests = g_tests_##VAR, \
            .userdata = nullptr, \
        }; \
        (void)bedrock; \
    } \
    FN_TEST_SUITE_FINI(Volta_##VAR) { (void)userdata; }

IMPL_VOLTA_TEST_SUITE(todo)
