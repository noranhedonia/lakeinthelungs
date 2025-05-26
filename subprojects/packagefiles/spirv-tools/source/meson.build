spirv_src = files(
    'assembly_grammar.cpp',
    'binary.cpp',
    'diagnostic.cpp',
    'disassemble.cpp',
    'enum_string_mapping.cpp',
    'ext_inst.cpp',
    'extensions.cpp',
    'libspirv.cpp',
    'name_mapper.cpp',
    'opcode.cpp',
    'operand.cpp',
    'parsed_operand.cpp',
    'print.cpp',
    'software_version.cpp',
    'spirv_endian.cpp',
    'spirv_optimizer_options.cpp',
    'spirv_reducer_options.cpp',
    'spirv_target_env.cpp',
    'spirv_validator_options.cpp',
    'table.cpp',
    'text.cpp',
    'text_handler.cpp',
    'util/bit_vector.cpp',
    'util/parse_number.cpp',
    'util/string_utils.cpp',
    'val/basic_block.cpp',
    'val/construct.cpp',
    'val/function.cpp',
    'val/instruction.cpp',
    'val/validate.cpp',
    'val/validate_adjacency.cpp',
    'val/validate_annotation.cpp',
    'val/validate_arithmetics.cpp',
    'val/validate_atomics.cpp',
    'val/validate_barriers.cpp',
    'val/validate_bitwise.cpp',
    'val/validate_builtins.cpp',
    'val/validate_capability.cpp',
    'val/validate_cfg.cpp',
    'val/validate_composites.cpp',
    'val/validate_constants.cpp',
    'val/validate_conversion.cpp',
    'val/validate_debug.cpp',
    'val/validate_decorations.cpp',
    'val/validate_derivatives.cpp',
    'val/validate_execution_limitations.cpp',
    'val/validate_extensions.cpp',
    'val/validate_function.cpp',
    'val/validate_id.cpp',
    'val/validate_image.cpp',
    'val/validate_instruction.cpp',
    'val/validate_interfaces.cpp',
    'val/validate_layout.cpp',
    'val/validate_literals.cpp',
    'val/validate_logicals.cpp',
    'val/validate_memory.cpp',
    'val/validate_memory_semantics.cpp',
    'val/validate_mesh_shading.cpp',
    'val/validate_misc.cpp',
    'val/validate_mode_setting.cpp',
    'val/validate_non_uniform.cpp',
    'val/validate_primitives.cpp',
    'val/validate_ray_query.cpp',
    'val/validate_ray_tracing.cpp',
    'val/validate_ray_tracing_reorder.cpp',
    'val/validate_scopes.cpp',
    'val/validate_small_type_uses.cpp',
    'val/validate_type.cpp',
    'val/validation_state.cpp',
)

spirv_generated_headers = []

spirv_generated_headers += custom_target('build-version.inc',
    command : [update_build_version, '@INPUT@', '@OUTPUT@'],
    input : '../CHANGES',
    output : 'build-version.inc',
    build_always_stale : true,
    build_by_default : true,
)

spirv_generated_headers += custom_target('spv-amd-shader-trinary-minmax.insts.inc',
    command : [
        gen_grammar_tables,
        '--extinst-vendor-grammar', '@INPUT@',
        '--vendor-insts-output', '@OUTPUT@',
        '--vendor-operand-kind-prefix=',
    ],
    input : file_unified1_extinst_spv_amd_shader_trinary_minmax_grammar_json,
    output : 'spv-amd-shader-trinary-minmax.insts.inc',
)

spirv_generated_headers += custom_target('nonsemantic.clspvreflection.insts.inc',
    command : [
        gen_grammar_tables,
        '--extinst-vendor-grammar', '@INPUT@',
        '--vendor-insts-output', '@OUTPUT@',
        '--vendor-operand-kind-prefix=',
    ],
    input : file_unified1_extinst_nonsemantic_clspvreflection_grammar_json,
    output : 'nonsemantic.clspvreflection.insts.inc',
)

spirv_generated_headers += custom_target('nonsemantic.vkspreflection.insts.inc',
    command : [
        gen_grammar_tables,
        '--extinst-vendor-grammar', '@INPUT@',
        '--vendor-insts-output', '@OUTPUT@',
        '--vendor-operand-kind-prefix=',
    ],
    input : file_unified1_extinst_nonsemantic_vkspreflection_grammar_json,
    output : 'nonsemantic.vkspreflection.insts.inc',
)

spirv_generated_headers += custom_target('nonsemantic.shader.debuginfo.100.insts.inc',
    command : [
        gen_grammar_tables,
        '--extinst-vendor-grammar', '@INPUT@',
        '--vendor-insts-output', '@OUTPUT@',
        '--vendor-operand-kind-prefix=',
    ],
    input : file_unified1_extinst_nonsemantic_shader_debuginfo_100_grammar_json,
    output : 'nonsemantic.shader.debuginfo.100.insts.inc',
)

spirv_generated_headers += custom_target('spv-amd-shader-explicit-vertex-parameter.insts.inc',
    command : [
        gen_grammar_tables,
        '--extinst-vendor-grammar', '@INPUT@',
        '--vendor-insts-output', '@OUTPUT@',
        '--vendor-operand-kind-prefix='
    ],
    input : file_unified1_extinst_spv_amd_shader_explicit_vertex_parameter_grammar_json,
    output : 'spv-amd-shader-explicit-vertex-parameter.insts.inc',
)

spirv_generated_headers += custom_target('spv-amd-shader-ballot.insts.inc',
    command : [
        gen_grammar_tables,
        '--extinst-vendor-grammar', '@INPUT@',
        '--vendor-insts-output', '@OUTPUT@',
        '--vendor-operand-kind-prefix=',
    ],
    input : file_unified1_extinst_spv_amd_shader_ballot_grammar_json,
    output : 'spv-amd-shader-ballot.insts.inc',
)

spirv_generated_headers += custom_target('spv-amd-gcn-shader.insts.inc',
    command : [
        gen_grammar_tables,
        '--extinst-vendor-grammar', '@INPUT@',
        '--vendor-insts-output', '@OUTPUT@',
        '--vendor-operand-kind-prefix=',
    ],
    input : file_unified1_extinst_spv_amd_gcn_shader_grammar_json,
    output : 'spv-amd-gcn-shader.insts.inc',
)

spirv_generated_headers += custom_target('DebugInfo.h',
    command : [
        gen_language_headers,
        '--extinst-grammar', '@INPUT@',
        '--extinst-output-path', '@OUTPUT@',
    ],
    input : file_unified1_extinst_debuginfo_grammar_json,
    output : 'DebugInfo.h',
)

spirv_generated_headers += custom_target('NonSemanticShaderDebugInfo100.h',
    command : [
        gen_language_headers,
        '--extinst-grammar', '@INPUT@',
        '--extinst-output-path', '@OUTPUT@',
    ],
    input : file_unified1_extinst_nonsemantic_shader_debuginfo_100_grammar_json,
    output : 'NonSemanticShaderDebugInfo100.h',
)

spirv_generated_headers += custom_target('OpenCLDebugInfo100.h',
    command : [
        gen_language_headers,
        '--extinst-grammar', '@INPUT@',
        '--extinst-output-path', '@OUTPUT@',
    ],
    input : file_unified1_extinst_opencl_debuginfo_100_grammar_json,
    output : 'OpenCLDebugInfo100.h',
)

spirv_generated_headers += custom_target('debuginfo.insts.inc',
    command : [
        gen_grammar_tables,
        '--extinst-vendor-grammar', '@INPUT@',
        '--vendor-insts-output', '@OUTPUT@',
        '--vendor-operand-kind-prefix=',
    ],
    input : file_unified1_extinst_debuginfo_grammar_json,
    output : 'debuginfo.insts.inc',
)

spirv_generated_headers += custom_target('extension_enum.inc and enum_string_mapping.inc',
    command : [
        gen_grammar_tables,
        '--spirv-core-grammar', '@INPUT0@',
        '--extinst-debuginfo-grammar', '@INPUT1@',
        '--extinst-cldebuginfo100', '@INPUT2@',
        '--extension-enum-output', '@OUTPUT0@',
        '--enum-string-mapping-output', '@OUTPUT1@',
        '--output-language=c++',
    ],
    input : [file_unified1_spirv_core_grammar_json, file_unified1_extinst_debuginfo_grammar_json, file_unified1_extinst_opencl_debuginfo_100_grammar_json],
    output : ['extension_enum.inc', 'enum_string_mapping.inc'],
)

spirv_generated_headers += custom_target('generators.inc',
    command : [
        gen_registry_tables,
        '--xml', '@INPUT@',
        '--generator-output', '@OUTPUT@',
    ],
    input : file_spir_v_xml,
    output : 'generators.inc',
)

spirv_generated_headers += custom_target('core.insts-unified1.inc and operand.kinds-unified1.inc',
    command : [
        gen_grammar_tables,
        '--spirv-core-grammar', '@INPUT0@',
        '--extinst-debuginfo-grammar', '@INPUT1@',
        '--extinst-cldebuginfo100-grammar', '@INPUT2@',
        '--core-insts-output', '@OUTPUT0@',
        '--operand-kinds-output', '@OUTPUT1@',
        '--output-language=c++',
    ],
    input : [file_unified1_spirv_core_grammar_json, file_unified1_extinst_debuginfo_grammar_json, file_unified1_extinst_opencl_debuginfo_100_grammar_json],
    output : ['core.insts-unified1.inc', 'operand.kinds-unified1.inc'],
)

spirv_generated_headers += custom_target('glsl.std.450.insts.inc',
    command : [
        gen_grammar_tables,
        '--extinst-glsl-grammar', '@INPUT@',
        '--glsl-insts-output', '@OUTPUT@',
        '--output-language=c++',
    ],
    input : file_unified1_extinst_glsl_std_450_grammar_json,
    output : 'glsl.std.450.insts.inc',
)

spirv_generated_headers += custom_target('opencl.std.insts.inc',
    command : [
        gen_grammar_tables,
        '--extinst-opencl-grammar', '@INPUT@',
        '--opencl-insts-output', '@OUTPUT@',
    ],
    input : file_unified1_extinst_opencl_std_100_grammar_json,
    output : 'opencl.std.insts.inc',
)

spirv_generated_headers += custom_target('opencl.debuginfo.100.insts.inc',
    command : [
        gen_grammar_tables,
        '--extinst-vendor-grammar', '@INPUT@',
        '--vendor-insts-output', '@OUTPUT@',
        '--vendor-operand-kind-prefix', 'CLDEBUG100_',
    ],
    input : file_unified1_extinst_opencl_debuginfo_100_grammar_json,
    output : 'opencl.debuginfo.100.insts.inc',
)

spirv_tools = library('spirv-tools',
    spirv_generated_headers,
    spirv_src,
    cpp_pch : 'meson-pch/pch.h',
    dependencies : spvhdr_dep,
    include_directories : [private_incdir, public_incdir],
    build_by_default : not meson.is_subproject(),
    override_options : cross_options,
)

spirv_tools_dep = declare_dependency(
    link_with : spirv_tools,
    include_directories : public_incdir,
    version : meson.project_version(),
)

spirv_tools_native = library('spirv-tools-native',
    spirv_generated_headers,
    spirv_src,
    cpp_pch : 'meson-pch/pch.h',
    dependencies : spvhdr_dep,
    include_directories : [private_incdir, public_incdir],
    build_by_default : false,
    native : true,
    override_options : native_options,
)

spirv_tools_native_dep = declare_dependency(
    link_with : spirv_tools_native,
    include_directories : public_incdir,
    version : meson.project_version(),
)

subdir('opt')
subdir('link')
subdir('reduce')
