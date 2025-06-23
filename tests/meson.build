test_sources = files(
    'main.c',
    'source/darray_test.c',
    'source/defer_test.c',
    'source/deque_test.c',
    'source/hadal_impl_test.c',
    'source/imgui_test.c',
    'source/moon_impl_test.c',
    'source/mpmc_ring_test.c',
    'source/slang_test.c',
    'source/soma_impl_test.c',
    'source/strbuf_test.c',
)

tests = executable(
    'tests', test_sources,
    dependencies: [ amwengine_dep ],
    install: false,
)
