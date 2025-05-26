project(
  'database_data_generator',
  'c',
  version: '1.0',
  default_options: ['buildtype=release', 'warning_level=3', 'default_library=static'],
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
bindir = run_command(pg_config, '--bindir', check: true).stdout().strip()

postgres_lib = meson.get_compiler('c').find_library('postgres', dirs: libdir, required: true)
python_dep = dependency('Python3')
cjson_dep = dependency('libcjson')

shared_module(
  'database_data_generator',
  'database_data_generator.c',
  'python_interface.c',
  'py_interaction.c',
  include_directories: [
    includedir_server,
    includedir,
    includedir_server_win32,
    includedir_server_win32_msvc,
  ],
  dependencies: [
    postgres_lib,
    python_dep,
    cjson_dep
    ],
  install: true,
  install_dir: pkglibdir,
  name_prefix: '',
)

install_data(
  'database_data_generator.control',
  'database_data_generator--1.0.sql',
  install_dir: sharedir / 'extension',
)

install_data(
  'c_interaction.py',
  'adress.py',
  'adress_generator.py',
  'personal_data.py',
  'personal_generator.py',
  install_dir: sharedir / 'database_data_generator',
)

pg_regress = find_program('pg_regress', dirs: [libdir / 'pgxs' / 'src' / 'test' / 'regress'])
test('regression', pg_regress,
  args: ['--bindir', bindir,
         '--inputdir', meson.current_source_dir() / 'test',
         '--user=postgres',
         'basic'])
