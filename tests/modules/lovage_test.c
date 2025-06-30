#include "../test_framework.h"

static struct test_case_details g_tests_todo[] = {
    //IMPL_TEST_CASE(Lovage_todo, todo),
};

#define IMPL_LOVAGE_TEST_SUITE(VAR) \
    FN_TEST_SUITE_INIT(Lovage_##VAR) \
    { \
        *out = (struct test_suite_details){ \
            .count = lake_arraysize(g_tests_##VAR), \
            .tests = g_tests_##VAR, \
            .userdata = nullptr, \
        }; \
        (void)bedrock; \
    } \
    FN_TEST_SUITE_FINI(Lovage_##VAR) { (void)userdata; }

IMPL_LOVAGE_TEST_SUITE(todo)
