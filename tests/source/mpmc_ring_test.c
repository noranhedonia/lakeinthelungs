#include "../framework.h"

static struct test_case_details g_tests[] = {
    //IMPL_TEST_CASE(MpmcRing, todo),
};

FN_TEST_SUITE_INIT(MpmcRing) 
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
        .userdata = nullptr,
    };
    (void)framework;
}

FN_TEST_SUITE_FINI(MpmcRing)
{
    (void)userdata;
}
