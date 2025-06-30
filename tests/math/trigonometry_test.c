#include "../test_framework.h"

static struct test_case_details g_tests[] = {
    //IMPL_TEST_CASE(Math_trigonometry, todo),
};

FN_TEST_SUITE_INIT(Math_trigonometry)
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
        .userdata = nullptr,
    };
    (void)bedrock;
}

FN_TEST_SUITE_FINI(Math_trigonometry)
{
    (void)userdata;
}
