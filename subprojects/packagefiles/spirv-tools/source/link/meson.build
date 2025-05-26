link_src = files(
    'linker.cpp',
)

spirv_tools_link = library('spirv-tools-link',
    spirv_generated_headers,
    link_src,
    dependencies : [spirv_tools_dep, spirv_tools_opt_dep, spvhdr_dep],
    include_directories : private_incdir,
    build_by_default : not meson.is_subproject(),
    override_options : cross_options,
)

spirv_tools_link_dep = declare_dependency(
    link_with : spirv_tools_link,
    include_directories : public_incdir,
    version : meson.project_version(),
)

spirv_tools_link_native = library('spirv-tools-link-native',
    spirv_generated_headers,
    link_src,
    dependencies : [spirv_tools_native_dep, spirv_tools_opt_native_dep, spvhdr_dep],
    include_directories : private_incdir,
    build_by_default : false,
    native : true,
    override_options : native_options,
)

spirv_tools_link_native_dep = declare_dependency(
    link_with : spirv_tools_link_native,
    include_directories : public_incdir,
    version : meson.project_version(),
)
