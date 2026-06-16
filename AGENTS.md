# AnnoTest — Codebase Guide

## Purpose

AnnoTest is a C++26 **annotation-based unit testing library**. It uses C++26 reflection and module system to let users mark test functions with `[[=Test{}]]` annotations instead of macros or boilerplate.

> **Naming inconsistency**: The project was renamed from `Cpp-Utils` to `AnnoTest`. The CMake target is `annotest`, the namespace is `annotest`, but the git remote and docs still reference `Cpp-Utils` / `cpputils`. Do not change the remote or docs without explicit direction.

## C++26 Module Layout

The library is a **CMake `STATIC` library** (`annotest`) built from C++26 modules. All source lives in `include/` — these are `.cxx` files that define `export module` units (not headers).

### Module tree (import graph)

```
annotest              — umbrella: imports all submodules
annotest:asserts      — assert* helpers (throw Error)
annotest:exceptions   — Error / Abort exception classes
annotest:expects      — expect* helpers (throw Abort)
annotest:death_test   — assertDeath / expectDeath (fork-based, Unix/macOS only)
annotest:posix        — fork / waitpid wrappers
annotest:tuple        — compile-time tuple for parameterized tests
annotest:utils        — reflection helpers: format(), enum_to_string(), getMembers(), etc.
```

Key CMake flags:
- **`-freflection`** — required (set globally in CMakeLists.txt line 23)
- **CXX standard 26** — with module feature `451f2fe2-a8a2-47c3-bc32-94786d8fc91b`
- **CMake 4.3+** required

## Directory Structure

| Path | Purpose |
|------|---------|
| `include/*.cxx` | Module source files (exported, no `.hpp` headers) |
| `tests/` | Self-tests: `test.cpp` exercises all asserts, expects, test suite, and OS annotations |
| `cmake/` | CMake install helpers: `DiscoverTests.cmake`, `DiscoverTestsRunner.cmake`, config template |
| `docs/` | Sphinx docs (Read the Docs). **Note: docs are outdated per user note** |
| `.github/workflows/build_and_test.yml` | CI: GCC 16 + CMake 4.2.3 + Ninja on Ubuntu 24.04 |

## Building

```bash
cmake -B build -DCMAKE_C_COMPILER=gcc-16 -DCMAKE_CXX_COMPILER=g++-16 \
       -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build/tests
```

CI uses `ENABLE_TESTING=ON` (maps to `-DBUILD_TESTS=ON`).

## Test Discovery System

The project has a **run-time test discovery** mechanism (no CMake 3.12+ `add_test` generator expressions):

1. Each test executable accepts `--list` → prints `SuiteName.` then indented test names
2. Each test executable accepts `--test-name <FullName>` → runs only that test and exits
3. `annotest_discover_tests(TARGET)` — CMake function that:
   - Builds the executable
   - Runs it with `--list` as a post-build step
   - Parses output, generates a `.cmake` file with individual `add_test()` entries
   - Injects the file via `TEST_INCLUDE_FILES` directory property

The runner script is `cmake/DiscoverTests.cmake`; the CMake function wrapper is in `cmake/DiscoverTestsRunner.cmake`.

## Coding Conventions

- **Formatter**: Google style, 100-column limit (`.clang-format`)
- **Clang-Tidy**: `modernize-*`, `readability-*`, `performance-*`, `bugprone-*`, `cppcoreguidelines-*`, `clang-analyzer-*` (excludes trailing return type rule)
- **Namespace**: Always `annotest::`
- **Exported symbols**: All public symbols are `export` in module files
- **Reflection helpers** in `annotest:utils` use `std::meta::*` C++26 reflection API

## CI

- **Runner**: `ubuntu-24.04`
- **Compiler**: GCC 16 (x64)
- **Generator**: Ninja
- **Build type**: Release
- **Tests run via**: `ctest` after `cmake --build`

## Key Dependencies on C++26 Features

- **Reflection TS / C++26 reflection** — `std::meta::*` API for member iteration, annotations, aggregate definition
- **Modules** — `import annotest;` style usage
- **`std::define_static_array`** — compile-time array construction
- **`template for`** loops — compile-time iteration over reflection results
- **Reflection literals** — `^^Test`, `:m:` subscript, `[:m:]` reflection extraction
