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

typedef s32 (LAKECALL *PFN_test_case)(void *userdata);
#define FN_TEST_CASE(SUITE, NAME, ARG) \
    s32 LAKECALL SUITE##_##NAME(ARG)

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
    void                       *userdata;
};

typedef void (LAKECALL *PFN_test_suite_init)(lake_framework const *framework, struct test_suite_details *out);
#define FN_TEST_SUITE_INIT(NAME) \
    void LAKECALL NAME##_test_suite_init(lake_framework const *framework, struct test_suite_details *out)

PFN_LAKE_WORK(PFN_test_suite_fini, void *);
#define FN_TEST_SUITE_FINI(NAME) \
    FN_LAKE_WORK(NAME##_test_suite_fini, void *userdata)
