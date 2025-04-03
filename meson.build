project(
  'my_new_extension',
  'c',
  version: '1.0',
  default_options: ['buildtype=release', 'warning_level=3'],
)
add_project_arguments('-DWIN32', language: 'c')

pg_config = find_program('pg_config')

includedir_server = run_command(pg_config, '--includedir-server', check: true).stdout().strip()
includedir_server_win32 = run_command(pg_config, '--includedir-server', check: true).stdout().strip() / 'port' / 'win32'
includedir_server_win32_msvc = run_command(pg_config, '--includedir-server', check: true).stdout().strip() / 'port' / 'win32_msvc'
includedir = run_command(pg_config, '--includedir', check: true).stdout().strip()
libdir = run_command(pg_config, '--libdir', check: true).stdout().strip()
pkglibdir = run_command(pg_config, '--pkglibdir', check: true).stdout().strip()
sharedir = run_command(pg_config, '--sharedir', check: true).stdout().strip()

postgres_lib = meson.get_compiler('c').find_library('postgres', dirs: libdir, required: true)

shared_module(
  'my_new_extension',
  'my_new_extension.c',
  include_directories: [
    includedir_server,
    includedir,
    includedir_server_win32,
    includedir_server_win32_msvc,
  ],
  dependencies: [postgres_lib],
  install: true,
  install_dir: pkglibdir,
  name_prefix: '',
)

install_data(
  'my_new_extension.control',
  'my_new_extension--1.0.sql',
  install_dir: sharedir / 'extension',
)
