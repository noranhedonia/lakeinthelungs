#include "../test_framework.h"

static struct test_case_details g_tests[] = {
    //IMPL_TEST_CASE(Graphics_mgpu_renderer, todo),
};

FN_TEST_SUITE_INIT(Graphics_mgpu_renderer)
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
        .userdata = nullptr,
    };
    (void)bedrock;
}

FN_TEST_SUITE_FINI(Graphics_mgpu_renderer)
{
    (void)userdata;
}
