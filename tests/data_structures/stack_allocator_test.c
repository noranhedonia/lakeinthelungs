#include "../framework.h"

static struct test_case_details g_tests[] = {
    //IMPL_TEST_CASE(DS_stack_allocator, todo),
};

FN_TEST_SUITE_INIT(DS_stack_allocator)
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
        .userdata = nullptr,
    };
    (void)bedrock;
}

FN_TEST_SUITE_FINI(DS_stack_allocator)
{
    (void)userdata;
}
