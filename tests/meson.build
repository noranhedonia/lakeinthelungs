test_sources = files(
    'test_framework.c',
#
    'bedrock/defer_test.c',
    'bedrock/drifter_test.c',
    'bedrock/file_system_test.c',
    'bedrock/job_system_test.c',
    'bedrock/machina_test.c',
    'bedrock/network_test.c',
    'bedrock/tagged_heap_test.c',
    'bedrock/truetype_test.c',
#
    'data_structures/arena_allocator_test.c',
    'data_structures/bitset_test.c',
    'data_structures/block_allocator_test.c',
    'data_structures/dagraph_test.c',
    'data_structures/darray_test.c',
    'data_structures/deque_test.c',
    'data_structures/hashmap_test.c',
    'data_structures/map_test.c',
    'data_structures/mpmc_ring_test.c',
    'data_structures/sparse_set_test.c',
    'data_structures/stack_allocator_test.c',
    'data_structures/strbuf_test.c',
    'data_structures/switch_list_test.c',
#
    'math/bits_test.c',
    'math/camera_test.c',
    'math/matrix_test.c',
    'math/quaternion_test.c',
    'math/simd_test.c',
    'math/trigonometry_test.c',
    'math/vector_test.c',
#
    'modules/hadal_test.c',
    'modules/hadean_test.c',
    'modules/ipomoea_test.c',
    'modules/lovage_test.c',
    'modules/moon_test.c',
    'modules/riven_test.c',
    'modules/soma_test.c',
    'modules/sorceress_test.c',
    'modules/volta_test.c',
#
    'audio/dsp_test.c',
    'audio/mixer_test.c',
    'audio/spatial_test.c',
    'audio/synth_test.c',
#
    'graphics/pipeline_builder_test.c',
    'graphics/render_graph_test.c',
    'graphics/renderer_test.c',
)

tests = executable(
    'tests', test_sources,
    dependencies: [ sorceress_dep ],
    install: false,
)
