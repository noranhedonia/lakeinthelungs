#include "../framework.h"

static struct test_case_details g_tests[] = {
    //IMPL_TEST_CASE(DS_hashmap, todo),
};

FN_TEST_SUITE_INIT(DS_hashmap)
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
        .userdata = nullptr,
    };
    (void)bedrock;
}

FN_TEST_SUITE_FINI(DS_hashmap)
{
    (void)userdata;
}
