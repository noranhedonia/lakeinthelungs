sources = [ 'bitwise.c', 'framing.c' ]

ogg = library('ogg'
    sources,
    config_types_h,
    include_directories: include_dir,
    build_by_default: not meson.is_subproject(),
)

ogg_dep = declare_dependency(
    link_width: ogg,
    include_directories: include_dir,
)

meson.override_dependency('ogg', ogg_dep)
