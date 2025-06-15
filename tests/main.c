#define LAKE_IN_THE_LUNGS_MAIN
#include "test_suites.h"

char const *construct_fiber_name(char const *suite, char const *test)
{
    usize const n0 = lake_strlen(suite);
    usize const n1 = lake_strlen(test);
    usize const sum = n0 + n1 + 2;

    char *buf = lake_drift_n(char, sum + 1);
    lake_memcpy(buf, suite, n0);
    buf[n0] = ':'; buf[n0 + 1] = ':';
    lake_memcpy(&buf[n0 + 2], test, n1);
    buf[sum] = '\0';

    return buf;
}

extern void _test_log_context(char const *file, s32 line)
{
    char const *fiber_name = lake_fiber_name();
    lake_log(-5, "        #[blue]@%s#[normal] from #[magenta]%d:%s#[normal]:", fiber_name, line, file);
}

struct main_test_suite {
    char const                 *name;
    PFN_test_suite_setup        setup;
    struct test_suite_details   details;
    atomic_u32                  status_ok;
    atomic_u32                  status_skip;
    atomic_u32                  status_fail;
};

#define IMPL_MAIN_TEST_SUITE(SUITE) \
    (struct main_test_suite){ \
        .name = #SUITE, \
        .setup = SUITE##_test_suite_setup, \
        .details = {0}, \
    }
static struct main_test_suite g_test_suites[] = {
    IMPL_MAIN_TEST_SUITE(Defer),
};
char const *g_run_target = nullptr;

struct run_test_work {
    struct test_case_details    details;
    atomic_u32                 *status_ok;
    atomic_u32                 *status_skip;
    atomic_u32                 *status_fail;
    f64                         dt;
};

static f64 g_dt_freq_reciprocal = 0.0;

FN_LAKE_WORK(run_test, struct run_test_work *work) 
{
    u64 const time_start = lake_rtc_counter();
    s32 const status = work->details.procedure();
    u64 const time_end = lake_rtc_counter();
    work->dt = ((f64)(time_end - time_start) * g_dt_freq_reciprocal);

    char const *msg_status = nullptr;
    char const *msg_color = nullptr;
    char const *msg_time = nullptr;
    switch (status) {
        case TEST_RESULT_OKAY: 
            msg_status = "#[green] OK #[normal]";
            msg_color = "normal";
            msg_time = "cyan";
            lake_atomic_add_explicit(work->status_ok, 1u, lake_memory_model_release);
            break;
        case TEST_RESULT_SKIPPED:
            msg_status = "#[yellow]SKIP#[normal]";
            msg_color = "grey";
            msg_time = "grey";
            lake_atomic_add_explicit(work->status_skip, 1u, lake_memory_model_release);
            break;
        case TEST_RESULT_FAILED:
            msg_status = "#[red]FAIL#[normal]";
            msg_color = "red";
            msg_time = "red";
            lake_atomic_add_explicit(work->status_fail, 1u, lake_memory_model_release);
            break;
    };
    lake_log(-5, "    [ %s ] #[%s]%s#[normal] (#[%s]%.6fms#[normal])", msg_status, msg_color, work->details.name, msg_time, 1000.0*work->dt);
}

static s32 run_test_suite(
        lake_framework const   *framework, 
        struct main_test_suite *suite)
{
    struct run_test_work *runs = nullptr;
    lake_work_details *work = nullptr;

    suite->setup(framework, &suite->details);

    /* print header */
    if (suite->details.count == 0) return TEST_RESULT_SKIPPED;

    lake_log(-6, "\nTest suite #[blue]%s#[normal] runs #[cyan]%u#[normal] test cases:", suite->name, suite->details.count);
    u32 const test_count = suite->details.count;

    runs = lake_drift_n(struct run_test_work, test_count);
    work = lake_drift_n(lake_work_details, test_count);

    for (u32 i = 0; i < test_count; i++) {
        runs[i].details     = suite->details.tests[i];
        runs[i].status_ok   = &suite->status_ok;
        runs[i].status_skip = &suite->status_skip;
        runs[i].status_fail = &suite->status_fail;
        work[i].procedure   = (PFN_lake_work)run_test;
        work[i].argument    = &runs[i];
        work[i].name        = construct_fiber_name(suite->name, runs[i].details.name);
    }
    lake_submit_work_and_yield(test_count, work);
    u32 const case_ok = lake_atomic_read(&suite->status_ok);
    u32 const case_skip = lake_atomic_read(&suite->status_skip);
    u32 const case_fail = lake_atomic_read(&suite->status_fail);
    f64 time_passed = 0.0;

    for (u32 i = 0; i < test_count; i++)
        time_passed += runs[i].dt;

    lake_log(-6, "    #[blue]%s#[normal] total results (#[cyan]%.6fms#[normal]): #[green]%u#[normal] okay, #[yellow]%u#[normal] skipped, "
            "#[red]%u#[normal] failed.", suite->name, 1000*time_passed, case_ok, case_skip, case_fail);
    return LAKE_SUCCESS;
}

FN_LAKE_FRAMEWORK(testing) 
{
    u32 suite_total_skip = 0;
    u32 case_total_ok = 0;
    u32 case_total_skip = 0;
    u32 case_total_fail = 0;
    u32 const suite_count = lake_arraysize(g_test_suites);
    u64 const time_start = lake_rtc_counter();
    g_dt_freq_reciprocal = 1.0f/(f64)lake_rtc_frequency();

    if (g_run_target) {
        /* run just this single test suite */
        s32 index = -1;

        for (u32 i = 0; i < suite_count; i++) {
            if (!strcmp(g_run_target, g_test_suites[i].name)) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            lake_log(-6, "Target test suite #[yellow]%s#[normal] does not exist.", g_run_target);
            return;
        }
        s32 status = run_test_suite(framework, &g_test_suites[index]);
        if (status == TEST_RESULT_SKIPPED) {
            lake_log(-6, "Target test suite #[yellow]%s#[normal] is skipped.", g_run_target);
            return;
        }
        return;
    }

    /* print test framework context */
    lake_log_enable_timestamps(true);
    lake_log(-6, "Welcome to the testing framework of #[blue]Lake in the Lungs#[normal] game engine. #[blue];333\n");
    lake_log_enable_timestamps(false);

    lake_log(-6, "A total of #[cyan]%u#[normal] test suites will be executed, using the following context:", suite_count);
    lake_log(-6, "    Worker threads: #[cyan]%u#[normal], running a #[cyan]%lu#[normal] capacity job queue.", framework->hints.worker_thread_count, 1lu << framework->hints.log2_work_count);
    lake_log(-6, "    Fibers: #[cyan]%u#[normal], each with a #[cyan]%u KiB#[normal] stack.", framework->hints.fiber_count, framework->hints.fiber_stack_size >> 10);
    
    /* execute test suites one by one */
    for (u32 i = 0; i < suite_count; i++) {
        struct main_test_suite *suite = &g_test_suites[i];

        lake_drift_push();
        s32 status = run_test_suite(framework, suite);
        lake_drift_pop();

        if (status == TEST_RESULT_SKIPPED) {
            lake_log(-6, "\nTest suite #[blue]%s#[normal] is skipped.", suite->name);
            suite_total_skip++;
            continue;
        }
        u32 const case_ok = lake_atomic_read(&suite->status_ok);
        u32 const case_skip = lake_atomic_read(&suite->status_skip);
        u32 const case_fail = lake_atomic_read(&suite->status_fail);
        case_total_ok += case_ok;
        case_total_skip += case_skip;
        case_total_fail += case_fail;
    }
    u64 const time_end = lake_rtc_counter();
    f64 const dt = ((f64)(time_end - time_start) * g_dt_freq_reciprocal); 

    /* print final raport */
    lake_log(-6, "\nComplete results from #[cyan]%u#[normal] test suites (#[yellow]%u#[normal] "
            "skipped), ran #[cyan]%u#[normal] test cases total (#[cyan]%.6fms#[normal]):", 
            suite_count - suite_total_skip, suite_total_skip, 
            case_total_ok + case_total_skip + case_total_fail, 1000.0*dt);

    if (case_total_ok)
        lake_log(-6, "    #[green]%u#[normal] okay", case_total_ok);
    if (case_total_skip)
        lake_log(-6, "    #[yellow]%u#[normal] skipped", case_total_skip);
    if (case_total_fail) {
        lake_log(-6, "    #[red]%u#[normal] failed :(", case_total_fail);
    } else {
        lake_log(-6, "All ran tests were #[green]OK#[normal]. :D");
    }
}

s32 lake_main(lake_framework *framework, s32 argc, const char **argv) 
{
    framework->app_name = "testing";
    framework->hints.memory_budget = 512lu*1024lu*1024lu;
    framework->hints.enable_debug_instruments = true;
    framework->hints.fiber_count = 64;
    framework->hints.fiber_stack_size = 128*1024;
    framework->hints.tagged_heap_count = 16;
    framework->hints.frames_in_flight = 3;
    framework->hints.log2_work_count = 11;
    lake_log_enable_context(false);
    lake_log_enable_threading(false);
    lake_log_enable_timestamps(false);
    lake_log_set_level(-3);
    if (argc > 1) g_run_target = argv[1];
    lake_abort(lake_in_the_lungs(testing, framework));
}
