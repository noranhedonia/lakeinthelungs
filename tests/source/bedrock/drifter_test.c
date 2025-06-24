#include "../../framework.h"

static struct test_case_details g_tests[] = {
    //IMPL_TEST_CASE(Drifter, todo),
};

FN_TEST_SUITE_INIT(Drifter) 
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
        .userdata = nullptr,
    };
    (void)framework;
}

FN_TEST_SUITE_FINI(Drifter)
{
    (void)userdata;
}
