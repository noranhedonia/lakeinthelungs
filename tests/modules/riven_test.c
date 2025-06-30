#include "../test_framework.h"

static struct test_case_details g_tests_core[] = {
    //IMPL_TEST_CASE(Riven_core, todo),
};

static struct test_case_details g_tests_entity[] = {
    //IMPL_TEST_CASE(Riven_entity, todo),
};

static struct test_case_details g_tests_component[] = {
    //IMPL_TEST_CASE(Riven_component, todo),
};

static struct test_case_details g_tests_system[] = {
    //IMPL_TEST_CASE(Riven_system, todo),
};

static struct test_case_details g_tests_archetype[] = {
    //IMPL_TEST_CASE(Riven_archetype, todo),
};

static struct test_case_details g_tests_query[] = {
    //IMPL_TEST_CASE(Riven_query, todo),
};

#define IMPL_RIVEN_TEST_SUITE(VAR) \
    FN_TEST_SUITE_INIT(Riven_##VAR) \
    { \
        *out = (struct test_suite_details){ \
            .count = lake_arraysize(g_tests_##VAR), \
            .tests = g_tests_##VAR, \
            .userdata = nullptr, \
        }; \
        (void)bedrock; \
    } \
    FN_TEST_SUITE_FINI(Riven_##VAR) { (void)userdata; }

IMPL_RIVEN_TEST_SUITE(core)
IMPL_RIVEN_TEST_SUITE(entity)
IMPL_RIVEN_TEST_SUITE(component)
IMPL_RIVEN_TEST_SUITE(system)
IMPL_RIVEN_TEST_SUITE(archetype)
IMPL_RIVEN_TEST_SUITE(query)
