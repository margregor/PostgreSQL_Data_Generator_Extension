An example extension for PostgreSQL 17
![example workflow](https://github.com/margregor/a_postgres_extension/actions/workflows/compile.yml/badge.svg)

Requires pg_config.exe to be in PATH

To compile:
```
meson setup builddir
meson compile -C builddir
```
in Native Tools Command Prompt for VS 2022

To install:
```
meson install -C builddir
```
Will probably require admin permissions

In Postgres:
```sql
CREATE EXTENSION my_new_extension;
SELECT add_one(4);
```
