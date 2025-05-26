project('asiosdk', 'cpp', license : 'Steinberg ASIO SDK Licensing Agreement Version 2.0.3', version : '2.3.3', meson_version : '>=0.50.0')

headers = files(
    'common/asio.h',
    'common/asiodrvr.h',
    'common/asiosys.h',
    'host/asiodrivers.h',
    'host/pc/asiolist.h',
    'iasiothiscallresolver.hpp',
)
install_headers(headers, subdir : 'asio')

sources = files(
    'common/asio.cpp',
    'host/asiodrivers.cpp',
    'host/pc/asiolist.cpp',
    'iasiothiscallresolver.cpp',
)

asiosdk = library(
    'asiosdk',
    sources,
    install: true,
)

asiosdk_dep = declare_dependency(
    link_with: asiosdk,
)
