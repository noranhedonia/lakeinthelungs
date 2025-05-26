assembly_arch = []
assembly_os = []
assembly_abi = []
assembly_asm = []

if host_machine.system() == 'windows'
    assembly_os = 'ms'
    assembly_abi = 'pe'
    assembly_asm = 'masm.asm'
    add_languages('masm')
elif host_machine.system() == 'darwin'
    assembly_os = 'sysv'
    assembly_abi = 'macho'
    assembly_asm = 'gas.s'
else
    assembly_os = 'sysv'
    assembly_abi = 'elf'
    assembly_asm = 'gas.s'
endif

if host_machine.cpu_family() == 'x86_64'
    if get_option('avx')
        pre_args += ['-mavx']
    endif
    if get_option('avx2')
        pre_args += ['-mavx2', '-mfma']
    endif
    assembly_arch = 'amd64'
elif host_machine.cpu_family() == 'aarch64'
    assembly_os = 'aapcs'
    assembly_arch = 'aarch64'
elif host_machine.cpu_family() == 'riscv64'
    assembly_arch = 'riscv'
    assembly_abi = 'rv64gc'
else
    error('CPU architecture \'' + host_machine.cpu_family() + '\' is not supported.')
endif

assembly_config = '_' + assembly_arch + '_' + assembly_os + '_' + assembly_abi + '_' + assembly_asm
engine_sources += files(
    'jump' + assembly_config,
    'make' + assembly_config,
)
