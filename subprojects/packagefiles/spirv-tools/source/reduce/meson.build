reduce_src = files(
    'change_operand_reduction_opportunity.cpp',
    'change_operand_to_undef_reduction_opportunity.cpp',
    'conditional_branch_to_simple_conditional_branch_opportunity_finder.cpp',
    'conditional_branch_to_simple_conditional_branch_reduction_opportunity.cpp',
    'merge_blocks_reduction_opportunity.cpp',
    'merge_blocks_reduction_opportunity_finder.cpp',
    'operand_to_const_reduction_opportunity_finder.cpp',
    'operand_to_dominating_id_reduction_opportunity_finder.cpp',
    'operand_to_undef_reduction_opportunity_finder.cpp',
    'reducer.cpp',
    'reduction_opportunity.cpp',
    'reduction_opportunity_finder.cpp',
    'reduction_pass.cpp',
    'reduction_util.cpp',
    'remove_block_reduction_opportunity.cpp',
    'remove_block_reduction_opportunity_finder.cpp',
    'remove_function_reduction_opportunity.cpp',
    'remove_function_reduction_opportunity_finder.cpp',
    'remove_instruction_reduction_opportunity.cpp',
    'remove_selection_reduction_opportunity.cpp',
    'remove_selection_reduction_opportunity_finder.cpp',
    'remove_struct_member_reduction_opportunity.cpp',
    'remove_unused_instruction_reduction_opportunity_finder.cpp',
    'remove_unused_struct_member_reduction_opportunity_finder.cpp',
    'simple_conditional_branch_to_branch_opportunity_finder.cpp',
    'simple_conditional_branch_to_branch_reduction_opportunity.cpp',
    'structured_construct_to_block_reduction_opportunity.cpp',
    'structured_construct_to_block_reduction_opportunity_finder.cpp',
    'structured_loop_to_selection_reduction_opportunity.cpp',
    'structured_loop_to_selection_reduction_opportunity_finder.cpp',
)

spirv_tools_reduce = library('spirv-tools-reduce',
    spirv_generated_headers,
    reduce_src,
    dependencies : [spirv_tools_dep, spirv_tools_opt_dep, spvhdr_dep],
    include_directories : private_incdir,
    build_by_default : not meson.is_subproject(),
    override_options : cross_options,
)

spirv_tools_reduce_dep = declare_dependency(
    link_with : spirv_tools_reduce,
    include_directories : public_incdir,
    version : meson.project_version(),
)

spirv_tools_reduce_native = library('spirv-tools-reduce-native',
    spirv_generated_headers,
    reduce_src,
    dependencies : [spirv_tools_native_dep, spirv_tools_opt_native_dep, spvhdr_dep],
    include_directories : private_incdir,
    build_by_default : false,
    native : true,
    override_options : native_options,
)

spirv_tools_reduce_native_dep = declare_dependency(
    link_with : spirv_tools_reduce_native,
    include_directories : public_incdir,
    version : meson.project_version(),
)
