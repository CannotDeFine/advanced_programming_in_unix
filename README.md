# Advanced Programming in the UNIX Environment Notes

This repository contains small C/C++ programs written while studying
_Advanced Programming in the UNIX Environment_.

The example programs live under `chapter*` directories. The bundled APUE
support code is kept in `apue.3e/`.

## Build

Build all chapter examples:

```sh
make
```

Executables are written to `bin/` with the same chapter path layout, for
example:

```sh
./bin/chapter01/01_ls .
```

The Makefile automatically builds the APUE helper library from
`apue.3e/lib/*.c` into `build/apue/libapue.a`, so chapter programs can use APUE
helpers such as `err_sys`, `path_alloc`, `open_max`, `set_fl`, and others
without adding each object file manually.

## Development

List all build targets:

```sh
make list
```

Generate `compile_commands.json` for clangd:

```sh
make clangd
```

Clean generated files:

```sh
make clean
```

Generated files such as `bin/`, `build/`, `compile_commands.json`, and APUE
library objects are ignored by git.
