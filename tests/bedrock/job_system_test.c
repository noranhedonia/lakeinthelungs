#include "../framework.h"

static struct test_case_details g_tests[] = {
    //IMPL_TEST_CASE(Bedrock_job_system, todo),
};

FN_TEST_SUITE_INIT(Bedrock_job_system)
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
        .userdata = nullptr,
    };
    (void)bedrock;
}

FN_TEST_SUITE_FINI(Bedrock_job_system)
{
    (void)userdata;
}
