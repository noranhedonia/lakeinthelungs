#include "../framework.h"

static struct test_case_details g_tests[] = {
    //IMPL_TEST_CASE(Graphics_pipeline_builder, todo),
};

FN_TEST_SUITE_INIT(Graphics_pipeline_builder)
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
        .userdata = nullptr,
    };
    (void)bedrock;
}

FN_TEST_SUITE_FINI(Graphics_pipeline_builder)
{
    (void)userdata;
}
