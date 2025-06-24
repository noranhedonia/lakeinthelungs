test_sources = files(
    'main.c',
    'source/bedrock/defer_test.c',
    'source/bedrock/drifter_test.c',
    'source/bedrock/job_system_test.c',
    'source/bedrock/tagged_heap_test.c',
    'source/data_structures/darray_test.c',
    'source/data_structures/deque_test.c',
    'source/data_structures/mpmc_ring_test.c',
    'source/data_structures/strbuf_test.c',
    'source/devel/imgui_test.c',
    'source/devel/slang_test.c',
    'source/math/bits_test.c',
    'source/hadal_impl_test.c',
    'source/moon_impl_test.c',
    'source/soma_impl_test.c',
)

tests = executable(
    'tests', test_sources,
    dependencies: [ amwengine_dep ],
    install: false,
)
