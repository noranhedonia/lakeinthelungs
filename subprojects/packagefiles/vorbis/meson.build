project('vorbis', 'c', version : '1.3.7', license : 'bsd-style')

ogg_dep = dependency('ogg', fallback : [ 'ogg', 'ogg_dep' ], required : true)

# configure.ac
lib_current = 4
lib_revision = 9
lib_age = 4
lib_soversion = lib_current - lib_age
lib_version = '@0@.@1@.@2@'.format(lib_soversion, lib_age, lib_revision)

file_lib_current = 6
file_lib_revision = 8
file_lib_age = 3
vorbisfile_soversion = file_lib_current - file_lib_age
vorbisfile_version = '@0@.@1@.@2@'.format(
  vorbisfile_soversion,
  file_lib_age,
  file_lib_revision,
)

enc_lib_current = 2
enc_lib_revision = 12
enc_lib_age = 0
vorbisenc_soversion = enc_lib_current - enc_lib_age
vorbisenc_version = '@0@.@1@.@2@'.format(
  vorbisenc_soversion,
  enc_lib_age,
  enc_lib_revision,
)

headers = files(
    'include/vorbis/codec.h',
    'include/vorbis/vorbisenc.h',
    'include/vorbis/vorbisfile.h',
)
install_headers(headers, subdir : 'vorbis')

include_dir = include_directories('include')

subdir('lib')
# these don't seem to build on windows
if host_machine.system() != 'windows'
    subdir('vq')
endif
