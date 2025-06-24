#include "../../framework.h"

s32 Defer_runs_in_reverse_order(void *) 
{
    s32 result = TEST_RESULT_FAILED;
    lake_defer_begin();
    lake_defer({
        result = TEST_RESULT_OKAY;
    });
    lake_defer({
        result = TEST_RESULT_FAILED;
    });
    lake_defer_return result;
}

static struct test_case_details g_tests[] = {
    IMPL_TEST_CASE(Defer, runs_in_reverse_order),
};

FN_TEST_SUITE_INIT(Defer) 
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
        .userdata = nullptr,
    };
    (void)framework;
}

FN_TEST_SUITE_FINI(Defer)
{
    (void)userdata;
}
