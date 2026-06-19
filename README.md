# AnnoTest

[![Build and Test](https://github.com/EJainDev/AnnoTest/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/EJainDev/AnnoTest/actions/workflows/build_and_test.yml)
[![Documentation](https://readthedocs.org/projects/annotest/badge/?version=stable)](https://annotest.readthedocs.io/)
![C++26](https://img.shields.io/badge/C%2B%2B-26-blue)
![CMake 4.3+](https://img.shields.io/badge/CMake-4.3%2B-orange)
![License](https://img.shields.io/badge/license-MIT-lightgray)

> Annotation-based unit testing for C++26 — write tests with `[[=Test{}]]`, not macros or `TEST_F` boilerplate.

AnnoTest is a header-free, macro-free testing framework for C++26. It leverages the C++26 reflection API (`std::meta::*`) and module system to let you mark test functions with compile-time annotations. No test macros, no string-based names, no `main()` boilerplate.

```cpp
import std;
import annotest;

using namespace annotest;

[Test]
void adds_two_numbers() {
    assertEqual(5, 2 + 3);
}
```

That's the entire test — a function with an annotation. AnnoTest discovers, runs, and reports it automatically.

---

## Quick Start

Build and run:

```bash
cmake -B build -DCMAKE_CXX_COMPILER=g++-16 -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

Then write your first test:

```cpp
// test.cpp
import annotest;
using namespace annotest;

[Test<"my first test">]
void basic_assertion() {
    assertEqual(42, 6 * 7);
    assertTrue(42 > 0);
    assertNear(3.1415, M_PI, 0.001);
}

// Optionally pass argc/argv to support --list and --test-name flags
int main(int argc, char** argv) {
    return test<MySuite>(argc, argv);
}
```

See the [full documentation](https://annotest.readthedocs.io/) for API reference, examples, and advanced usage.

---

## Installation

### CMake FetchContent (recommended for consumers)

```cmake
FetchContent_Declare(
    annotest
    GIT_REPOSITORY https://github.com/EJainDev/AnnoTest.git
    GIT_TAG v1.0.0.0
)
FetchContent_MakeAvailable(annotest)

add_executable(my_tests test.cpp)
target_link_libraries(my_tests PRIVATE annotest::annotest)
```

### Install from source

```bash
git clone https://github.com/EJainDev/AnnoTest.git
cd AnnoTest

cmake -B build \
    -DCMAKE_C_COMPILER=gcc-16 \
    -DCMAKE_CXX_COMPILER=g++-16 \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build
cmake --install build
```

Consumers then use:

```cmake
find_package(AnnoTest REQUIRED)
target_link_libraries(my_app PRIVATE annotest::annotest)
```

### Requirements

| Component | Minimum | Notes |
|-----------|---------|-------|
| Compiler | GCC 16 | C++26 reflection required |
| CMake | 4.3 | Required for `CXX_MODULES` file set |
| C++ Standard | 26 | Mandatory |
| OS | Linux, macOS | Death tests require fork (Unix/macOS) |

> [!NOTE]
> AnnoTest uses C++26 reflection (`std::meta::*`) and the C++26 module system.
> A compiler with experimental C++26 support is required.

---

## Features

### Annotation-based test definition

Mark test functions with `[[=Test{}]]` — no macros, no string-based test names needed.

| Annotation | Syntax | Description |
|------------|--------|-------------|
| **Test** | `[Test]` | Marks a function as a test case |
| **Named test** | `[Test<"custom name">]` | Overrides the display name |
| **Disabled** | `[Test{.disabled = true}]` | Skips the test without deleting it |
| **BeforeEach** | `[BeforeEach]` | Runs before every test in the suite |
| **AfterEach** | `[AfterEach]` | Runs after every test in the suite |
| **BeforeAll** | `[BeforeAll]` | Runs once before the entire suite |
| **AfterAll** | `[AfterAll]` | Runs once after the entire suite |
| **Parameterize** | `[Parameterize{tuple(1,2), tuple(3,4)}]` | Runs with each parameter set |
| **RequiresOS** | `[RequiresOS{OS::Linux}]` | Skips test on other platforms |
| **DisallowOS** | `[DisallowOS{OS::Windows}]` | Skips test on listed platforms |

### Assertions (fail-fast)

Throw `Error` and stop the current test immediately:

```cpp
assertEqual(5, 2 + 3);
assertNotEqual(1, 2);
assertTrue(result.valid());
assertNear(3.14, M_PI, 0.01);
assertThrows<std::out_of_range>([] { vec.at(99); });
assertContractViolation([] { /* triggers a contract */ });
```

### Expectations (continue on failure)

Throw `Abort` instead — use when you want to keep testing after a failure:

```cpp
expectEqual(a, b);       // Assert equality, continue on failure
expectThrows<std::bad_alloc>([] { /* may throw */ });
```

### Parameterized tests

Run the same logic with different inputs:

```cpp
[Parameterize{tuple(1, 2), tuple(10, 20), tuple(100, 200)}]
void sums_match(int a, int b) {
    assertEqual(a + b, add(a, b));
}
```

### Death tests

Verify that a function causes process termination (Unix/macOS only):

```cpp
[DeathTest]
void crashes_on_null() {
    assertDeath([] { *reinterpret_cast<int*>(0) = 42; });
}
```

### Contract testing

Assert that C++26 `pre`/`post` contracts fire as expected:

```cpp
assertContractViolation([] { /* triggers a pre/post violation */ });
assertNoContractViolation([] { /* no contract fires */ });
```

---

## CMake Integration

### Test Discovery

AnnoTest ships with a built-in CMake test discovery function — no manual `add_test()` calls:

```cmake
add_executable(my_tests test.cpp)
target_link_libraries(my_tests PRIVATE annotest::annotest)

# Automatically registers each discovered test as a CTest entry
annotest_discover_tests(my_tests)
```

### Runtime CLI Flags

Every AnnoTest-linked executable supports:

| Flag | Description |
|------|-------------|
| `--list` | Print suite name and indented test names |
| `--test-name <Name>` | Run only the specified test |

### CMake Presets

The project includes `CMakePresets.json` with pre-configured Debug and Release builds:

```bash
cmake --preset release-gcc-16.1
cmake --build --preset build-release
```

---

## Comparison

### AnnoTest vs GoogleTest

| Feature | AnnoTest | GoogleTest |
|---------|----------|------------|
| Test definition | `[Test]` annotation | `TEST_F(TestCase, Name)` |
| Assertions | `assertEqual(5, 2+3)` | `ASSERT_EQ(5, 2+3)` |
| Macros | **None** | Heavy macro usage |
| C++26 Modules | `import annotest;` | Header-only |
| Reflection | Compile-time via `std::meta` | Manual registration |

### AnnoTest vs Catch2

| Feature | AnnoTest | Catch2 |
|---------|----------|--------|
| Syntax | Annotation-based | `TEST_CASE("name")` |
| Assertions | `assertEqual` / `expectEqual` | `REQUIRE` / `CHECK` |
| Test names | From function names | String literal required |
| Modern C++ | C++26 modules | C++11+ (headers) |

---

## Project Status

AnnoTest is under active development. Current implementation supports:

- ✅ Annotation-based test definitions
- ✅ Assertions and expectations (fail-fast and continue-on-failure)
- ✅ Lifecycle hooks (BeforeEach, AfterEach, BeforeAll, AfterAll)
- ✅ Parameterized tests
- ✅ Death tests (Unix/macOS)
- ✅ Contract violation assertions
- ✅ CMake test discovery
- ✅ C++26 modules

Planned:

- ⏳ Windows support (death tests via spawn)
- ⏳ CI coverage across more compiler versions
- ⏳ Richer failure message formatting

> [!NOTE]
> The API may change before the first stable release.

---

## Module Architecture

```
annotest              — umbrella module: import annotest;
├── annotest:tuple    — compile-time tuples for parameterized tests
├── annotest:utils    — reflection helpers (format, enum_to_string)
├── annotest:exceptions — Error / Abort exception classes
├── annotest:asserts  — assert* helpers (throw Error)
├── annotest:expects  — expect* helpers (throw Abort)
├── annotest:death_test — assertDeath / expectDeath (fork-based)
├── annotest:posix    — fork / waitpid wrappers
└── annotest:contracts — contract violation handling
```

---

## Resources

- [Documentation](https://annotest.readthedocs.io/) — full API reference and examples
- [Contributing](CONTRIBUTING.md) — how to get started
- [GitHub Issues](https://github.com/EJainDev/AnnoTest/issues) — bug reports and feature requests

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for the full guide.

Quick start for contributors:

```bash
cmake -B build -DCMAKE_CXX_COMPILER=g++-16 -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build
```

---

## License

AnnoTest is licensed under the MIT License — see [LICENSE](LICENSE) for details.
