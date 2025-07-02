#include "../test_framework.h"

static struct test_case_details g_tests[] = {
    //IMPL_TEST_CASE(DS_mpmc, todo),
};

FN_TEST_SUITE_INIT(DS_mpmc)
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
        .userdata = nullptr,
    };
    (void)bedrock;
}

FN_TEST_SUITE_FINI(DS_mpmc)
{
    (void)userdata;
}
