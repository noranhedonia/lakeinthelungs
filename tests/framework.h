#include <lake/inthelungs.h>

enum : s32 {
    TEST_RESULT_OKAY = 0,
    TEST_RESULT_SKIPPED = 1,
    TEST_RESULT_FAILED = 2,
};

#ifdef LAKE_PLATFORM_WINDOWS
    #define FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
    #define FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

extern void _test_log_context(char const *file, s32 line);
#define test_log_context() _test_log_context(FILENAME, __LINE__)
#define test_log(...) lake_log(-5, "        " __VA_ARGS__)

typedef s32 (LAKECALL *PFN_test_case)(void);
#define FN_TEST_CASE(SUITE, NAME) \
    s32 LAKECALL SUITE##_##NAME(void)

struct test_case_details {
    PFN_test_case   procedure;
    char const     *name;
};
#define IMPL_TEST_CASE(SUITE, NAME) \
    (struct test_case_details){ \
        .procedure = (PFN_test_case)SUITE##_##NAME, \
        .name = #NAME, \
    }

struct test_suite_details {
    u32                         count;
    struct test_case_details   *tests;
};

typedef void (LAKECALL *PFN_test_suite_setup)(lake_framework const *framework, struct test_suite_details *out);
#define FN_TEST_SUITE(NAME) \
    void LAKECALL NAME##_test_suite_setup(lake_framework const *framework, struct test_suite_details *out)
