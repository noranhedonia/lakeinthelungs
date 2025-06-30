#include "../test_framework.h"

s32 Bedrock_defer_runs_in_reverse_order(void *) 
{
    s32 result = TEST_RESULT_FAILED;
    lake_defer_begin;
    lake_defer({
        result = TEST_RESULT_OKAY;
    });
    lake_defer({
        result = TEST_RESULT_FAILED;
    });
    lake_defer_return result;
}

static struct test_case_details g_tests[] = {
    IMPL_TEST_CASE(Bedrock_defer, runs_in_reverse_order),
};

FN_TEST_SUITE_INIT(Bedrock_defer)
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
        .userdata = nullptr,
    };
    (void)bedrock;
}

FN_TEST_SUITE_FINI(Bedrock_defer)
{
    (void)userdata;
}
