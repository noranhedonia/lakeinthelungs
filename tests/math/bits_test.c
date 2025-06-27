#include "../framework.h"

static struct test_case_details g_tests[] = {
    //IMPL_TEST_CASE(Math_bits, todo),
};

FN_TEST_SUITE_INIT(Math_bits)
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
        .userdata = nullptr,
    };
    (void)bedrock;
}

FN_TEST_SUITE_FINI(Math_bits)
{
    (void)userdata;
}
