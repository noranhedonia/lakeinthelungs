#define LAKE_IN_THE_LUNGS_MAIN
#include "test_suites.h"

char const *construct_fiber_name(char const *suite, char const *test)
{
    usize const n0 = lake_strlen(suite);
    usize const n1 = lake_strlen(test);
    usize const sum = n0 + n1 + 2;

    char *buf = lake_drift_n(char, sum + 1);
    lake_memcpy(buf, suite, n0);
    for (s32 i = 0; i < 2; buf[n0 + i++] = ':');
    lake_memcpy(&buf[n0 + 2], test, n1);
    buf[sum] = '\0';

    return buf;
}

extern void _test_log_context(char const *file, s32 line)
{
    char const *fiber_name = lake_fiber_name();
    lake_log(-5, "        #[blue]@%s#[normal] from #[magenta]%d:%s#[normal]:", fiber_name, line, file);
}

struct test_suite_entry {
    char const                 *name;
    PFN_test_suite_init         init;
    PFN_test_suite_fini         fini;
    struct test_suite_details   details;
    atomic_u32                  status_ok;
    atomic_u32                  status_skip;
    atomic_u32                  status_fail;
};

#define TEST_SUITE_ENTRY(SUITE) \
    (struct test_suite_entry){ \
        .name = #SUITE, \
        .init = SUITE##_test_suite_init, \
        .fini = SUITE##_test_suite_fini, \
        .details = {0}, \
    }
static struct test_suite_entry g_test_suites[] = {
    /* bedrock */
    TEST_SUITE_ENTRY(Bedrock_defer),
    TEST_SUITE_ENTRY(Bedrock_drifter),
    TEST_SUITE_ENTRY(Bedrock_file_system),
    TEST_SUITE_ENTRY(Bedrock_job_system),
    TEST_SUITE_ENTRY(Bedrock_machina),
    TEST_SUITE_ENTRY(Bedrock_network),
    TEST_SUITE_ENTRY(Bedrock_tagged_heap),

    /* data structures */
    TEST_SUITE_ENTRY(DS_arena_allocator),
    TEST_SUITE_ENTRY(DS_bitset),
    TEST_SUITE_ENTRY(DS_block_allocator),
    TEST_SUITE_ENTRY(DS_darray),
    TEST_SUITE_ENTRY(DS_deque),
    TEST_SUITE_ENTRY(DS_hashmap),
    TEST_SUITE_ENTRY(DS_map),
    TEST_SUITE_ENTRY(DS_mpmc_ring),
    TEST_SUITE_ENTRY(DS_sparse_set),
    TEST_SUITE_ENTRY(DS_stack_allocator),
    TEST_SUITE_ENTRY(DS_strbuf),
    TEST_SUITE_ENTRY(DS_switch_list),

    /* math */
    TEST_SUITE_ENTRY(Math_bits),
    TEST_SUITE_ENTRY(Math_camera),
    TEST_SUITE_ENTRY(Math_mat2),
    TEST_SUITE_ENTRY(Math_mat3),
    TEST_SUITE_ENTRY(Math_mat4),
    TEST_SUITE_ENTRY(Math_quat),
    TEST_SUITE_ENTRY(Math_simd),
    TEST_SUITE_ENTRY(Math_vec4),

/* display backend implementations */
#ifdef HADAL_WIN32
    TEST_SUITE_ENTRY(HadalImpl_win32),
#endif /* HADAL_WIN32 */
#ifdef HADAL_COCOA
    TEST_SUITE_ENTRY(HadalImpl_cocoa),
#endif /* HADAL_COCOA */
#ifdef HADAL_UIKIT
    TEST_SUITE_ENTRY(HadalImpl_uikit),
#endif /* HADAL_UIKIT */
#ifdef HADAL_ANDROID
    TEST_SUITE_ENTRY(HadalImpl_android),
#endif /* HADAL_ANDROID */
#ifdef HADAL_HAIKU
    TEST_SUITE_ENTRY(HadalImpl_haiku),
#endif /* HADAL_HAIKU */
#ifdef HADAL_HTML5
    TEST_SUITE_ENTRY(HadalImpl_html5),
#endif /* HADAL_HTML5 */
#ifdef HADAL_WAYLAND
    TEST_SUITE_ENTRY(HadalImpl_wayland),
#endif /* HADAL_WAYLAND */
#ifdef HADAL_XCB
    TEST_SUITE_ENTRY(HadalImpl_xcb),
#endif /* HADAL_XCB */
#ifdef HADAL_KMS
    TEST_SUITE_ENTRY(HadalImpl_kms),
#endif /* HADAL_KMS */
    TEST_SUITE_ENTRY(HadalImpl_headless),

/* XR backend implementations */
#ifdef HADAL_OPENXR
    TEST_SUITE_ENTRY(HadeanImpl_openxr),
#endif /* HADAL_OPENXR */
    TEST_SUITE_ENTRY(HadeanImpl_headless),

/* rendering backend implementations */
#ifdef MOON_D3D12
    TEST_SUITE_ENTRY(MoonImpl_d3d12),
#endif /* MOON_D3D12 */
#ifdef MOON_METAL
    TEST_SUITE_ENTRY(MoonImpl_metal),
#endif /* MOON_METAL */
#ifdef MOON_WEBGPU
    TEST_SUITE_ENTRY(MoonImpl_webgpu),
#endif /* MOON_WEBGPU */
#ifdef MOON_VULKAN
    TEST_SUITE_ENTRY(MoonImpl_vulkan),
#endif /* MOON_VULKAN */
    TEST_SUITE_ENTRY(MoonImpl_mock),

/* audio backend implementations */
#ifdef SOMA_ASIO
    TEST_SUITE_ENTRY(SomaImpl_asio),
#endif /* SOMA_ASIO */
#ifdef SOMA_WASAPI
    TEST_SUITE_ENTRY(SomaImpl_wasapi),
#endif /* SOMA_WASAPI */
#ifdef SOMA_XAUDIO2
    TEST_SUITE_ENTRY(SomaImpl_xaudio2),
#endif /* SOMA_XAUDIO2 */
#ifdef SOMA_COREAUDIO
    TEST_SUITE_ENTRY(SomaImpl_coreaudio),
#endif /* SOMA_COREAUDIO */
#ifdef SOMA_AAUDIO
    TEST_SUITE_ENTRY(SomaImpl_aaudio),
#endif /* SOMA_AAUDIO */
#ifdef SOMA_WEBAUDIO
    TEST_SUITE_ENTRY(SomaImpl_webaudio),
#endif /* SOMA_WEBAUDIO */
#ifdef SOMA_PIPEWIRE
    TEST_SUITE_ENTRY(SomaImpl_pipewire),
#endif /* SOMA_PIPEWIRE */
#ifdef SOMA_PULSEAUDIO
    TEST_SUITE_ENTRY(SomaImpl_pulseaudio),
#endif /* SOMA_PULSEAUDIO */
#ifdef SOMA_JACK
    TEST_SUITE_ENTRY(SomaImpl_jack),
#endif /* SOMA_JACK */
#ifdef SOMA_ALSA
    TEST_SUITE_ENTRY(SomaImpl_alsa),
#endif /* SOMA_ALSA */
    TEST_SUITE_ENTRY(SomaImpl_dummy),

    /* ui */
    TEST_SUITE_ENTRY(Lovage_todo),

    /* entity-component-system */
    TEST_SUITE_ENTRY(Riven_core),
    TEST_SUITE_ENTRY(Riven_entity),
    TEST_SUITE_ENTRY(Riven_component),
    TEST_SUITE_ENTRY(Riven_system),
    TEST_SUITE_ENTRY(Riven_archetype),
    TEST_SUITE_ENTRY(Riven_query),

    /* physics */
    TEST_SUITE_ENTRY(Volta_todo),

    /* animation */
    TEST_SUITE_ENTRY(IpomoeaAlba_todo),

    /* audio */
    TEST_SUITE_ENTRY(Audio_dsp),
    TEST_SUITE_ENTRY(Audio_mixer),
    TEST_SUITE_ENTRY(Audio_spatial),
    TEST_SUITE_ENTRY(Audio_synth),

    /* graphics */
    TEST_SUITE_ENTRY(Graphics_pipeline_builder),
    TEST_SUITE_ENTRY(Graphics_render_graph),
    TEST_SUITE_ENTRY(Graphics_renderer),
};
char const *g_run_target = nullptr;

struct run_test_work {
    void                       *userdata;
    struct test_case_details    details;
    atomic_u32                 *status_ok;
    atomic_u32                 *status_skip;
    atomic_u32                 *status_fail;
};
static f64 g_dt_freq_reciprocal = 0.0;

FN_LAKE_WORK(run_test, struct run_test_work *work) 
{
    u64 const time_start = lake_rtc_counter();
    s32 const status = work->details.procedure(work->userdata);
    u64 const time_end = lake_rtc_counter();
    f64 const dt = ((f64)(time_end - time_start) * g_dt_freq_reciprocal);

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
    lake_log(-5, "    [ %s ] #[%s]%s#[normal] (#[%s]%.6fms#[normal])", msg_status, msg_color, work->details.name, msg_time, 1000.0*dt);
}

static s32 run_test_suite(
        lake_bedrock const      *bedrock, 
        struct test_suite_entry *suite)
{
    u64 const time_start = lake_rtc_counter();
    struct run_test_work *runs = nullptr;
    lake_work_details *work = nullptr;

    suite->init(bedrock, &suite->details);

    /* print header */
    if (suite->details.count == 0) return TEST_RESULT_SKIPPED;

    lake_log(-6, "\nTest suite #[blue]%s#[normal] runs #[cyan]%u#[normal] test cases:", suite->name, suite->details.count);
    u32 const test_count = suite->details.count;

    runs = lake_drift_n(struct run_test_work, test_count);
    work = lake_drift_n(lake_work_details, test_count);

    for (u32 i = 0; i < test_count; i++) {
        runs[i].details     = suite->details.tests[i];
        runs[i].userdata    = suite->details.userdata;
        runs[i].status_ok   = &suite->status_ok;
        runs[i].status_skip = &suite->status_skip;
        runs[i].status_fail = &suite->status_fail;
        work[i].procedure   = (PFN_lake_work)run_test;
        work[i].argument    = &runs[i];
        work[i].name        = construct_fiber_name(suite->name, runs[i].details.name);
    }
    lake_submit_work_and_yield(test_count, work);
    u64 const time_end = lake_rtc_counter();
    f64 const dt = ((f64)(time_end - time_start) * g_dt_freq_reciprocal);
    u32 const case_ok = lake_atomic_read(&suite->status_ok);
    u32 const case_skip = lake_atomic_read(&suite->status_skip);
    u32 const case_fail = lake_atomic_read(&suite->status_fail);

    lake_log(-6, "    #[blue]%s#[normal] total results (#[cyan]%.6fms#[normal]): #[green]%u#[normal] okay, #[yellow]%u#[normal] skipped, "
            "#[red]%u#[normal] failed.\n", suite->name, 1000*dt, case_ok, case_skip, case_fail);

    suite->fini(suite->details.userdata);
    return LAKE_SUCCESS;
}

void LAKECALL testing(void *, lake_bedrock const *bedrock) 
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
            lake_log(-6, "Target test suite #[magenta]%s#[normal] does not exist.", g_run_target);
            return;
        }
        s32 status = run_test_suite(bedrock, &g_test_suites[index]);
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
    lake_log(-6, "    Worker threads: #[cyan]%u#[normal], running a #[cyan]%lu#[normal] capacity job queue.", bedrock->hints.worker_thread_count, 1lu << bedrock->hints.log2_work_count);
    lake_log(-6, "    Fibers: #[cyan]%u#[normal], each with a #[cyan]%u KiB#[normal] stack.", bedrock->hints.fiber_count, bedrock->hints.fiber_stack_size >> 10);
    
    /* execute test suites one by one */
    bool last_was_skipped = true;
    for (u32 i = 0; i < suite_count; i++) {
        struct test_suite_entry *suite = &g_test_suites[i];

        lake_drift_push();
        s32 status = run_test_suite(bedrock, suite);
        lake_drift_pop();

        last_was_skipped = false;
        if (status == TEST_RESULT_SKIPPED) {
            lake_log(-6, "Test suite #[yellow]%s#[normal] is skipped.", suite->name);
            suite_total_skip++;
            last_was_skipped = true;
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
    lake_log(-6, "%sComplete results from #[cyan]%u#[normal] test suites (#[yellow]%u#[normal] "
            "skipped), ran #[cyan]%u#[normal] test cases total:", 
            last_was_skipped ? "\n" : "", suite_count - suite_total_skip, suite_total_skip, 
            case_total_ok + case_total_skip + case_total_fail);

    lake_log(-6, "    #[green]%u#[normal] okay, #[yellow]%u#[normal] skipped, #[red]%u#[normal] failed. "
            "(#[cyan]%.6fms#[normal]).", case_total_ok, case_total_skip, case_total_fail, 1000.0*dt);
    if (!case_total_fail)
        lake_log(-6, "All ran tests were #[green]OK#[normal]. :D");
}

s32 lake_main(lake_bedrock *bedrock, s32 argc, const char **argv) 
{
    bedrock->app_name = "Lake in the Lungs Testing";
    bedrock->hints.memory_budget = 512lu*1024lu*1024lu;
    bedrock->hints.enable_debug_instruments = false;
    bedrock->hints.networking_offline = false;
    bedrock->hints.fiber_count = 128;
    bedrock->hints.fiber_stack_size = 128*1024;
    bedrock->hints.tagged_heap_count = 16;
    bedrock->hints.frames_in_flight = 3;
    bedrock->hints.log2_work_count = 11;
    lake_log_enable_colors(true);
    lake_log_enable_context(false);
    lake_log_enable_threading(false);
    lake_log_enable_timestamps(false);
    lake_log_set_level(-3);
    if (argc > 1) g_run_target = argv[1];
    return lake_in_the_lungs(testing, nullptr, bedrock);
}
