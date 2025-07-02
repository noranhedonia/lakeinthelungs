#include "../test_framework.h"

static struct test_case_details g_tests[] = {
    //IMPL_TEST_CASE(Bedrock_magic, todo),
};

FN_TEST_SUITE_INIT(Bedrock_magic)
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
        .userdata = nullptr,
    };
    (void)bedrock;
}

FN_TEST_SUITE_FINI(Bedrock_magic)
{
    (void)userdata;
}
