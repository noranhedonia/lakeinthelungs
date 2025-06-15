#include "../framework.h"

s32 Defer_todo(void) 
{
    // lake_defer_begin();

    // lake_defer({
    //     s32 todo; (void)todo;
    // });

    // lake_defer_return TEST_RESULT_OKAY;
    // lake_defer_end();
    return TEST_RESULT_OKAY;
}

s32 Defer_kotki_wowoow(void) 
{
    test_log_context();
    test_log("Wooah this test failed for some reason duhhh... :c");
    return TEST_RESULT_FAILED;
}

s32 Defer_miau_miauu_miau(void) 
{
    return TEST_RESULT_OKAY;
}

s32 Defer_owowooaaaaaahhhoo_waofenafo(void) 
{
    return TEST_RESULT_SKIPPED;
}

static struct test_case_details g_tests[] = {
    IMPL_TEST_CASE(Defer, todo),
    IMPL_TEST_CASE(Defer, miau_miauu_miau),
    IMPL_TEST_CASE(Defer, kotki_wowoow),
    IMPL_TEST_CASE(Defer, owowooaaaaaahhhoo_waofenafo),
};

FN_TEST_SUITE(Defer) 
{
    *out = (struct test_suite_details){
        .count = lake_arraysize(g_tests),
        .tests = g_tests,
    };
    (void)framework;
}
