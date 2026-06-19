# AnnoTest
[![Build and Test](https://github.com/EJainDev/AnnoTest/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/EJainDev/AnnoTest/actions/workflows/build_and_test.yml) ![Documentation](https://app.readthedocs.org/projects/annotest/badge/?version=stable)

---

This is a C++26 annotation-based, testing library that simplifies test development by adding annotations to create tests.

# Testing

With C++26, we now have support for annotations, similar to those in Java. We can use annotations with the `[[=AggregateType{constructor, parameters}]]` syntax. That means no more wrapper macros or wrapper functions, just a simple `[[=Test{}]]` annotation on your function and the library will handle it for you! This library's goal is to make testing simpler and tests more readable. This also provides automated CMake test discovery with the `annotest_discover_tests` command.

## Versus Other Testing Frameworks

### GoogleTest

Pros:
- No macros
- No need to memorize names (`ASSERT_NE` vs `assertNotEqual`)
- Easy to disable with `[[= Test{.disabled = true}]]` annotation

Cons:
- No automatic main method
- No Windows support for death tests (note: GoogleTest death tests also do not work on Windows)
- No mocking support
- Text output only

But these will be worked on with contributions. If you feel like you can change one of these, create a PR and let's add it!

### Catch2

Pros:
- No macros
- More versatile assertions and expectations rather than just REQUIRE and CHECK

Cons:
- No spaces in test names

## Getting Started

This is a C++20 module library so all you have to do is write: `import annotest;` to get all the features. We recommend using `using namespace annotest` to avoid having to type it as a prefix for all annotations.

Checkout the [basic example](examples/example01_basic.cpp) for more detail. Here is a quick brief of what everything does:

- `[[=Test{}]]` annotation: Marks a function as a test
- `[[=BeforeEach{}]]` annotation: Runs once before every test
- `[[=BeforeAll{}]]` annotation: Runs once before any of the tests. ***IMPORTANT:** When running via CTest, each test is executed in a separate process, so BeforeAll runs once per test executable (not once per test suite). Use `--test-name <Name>` to run a single test directly for true suite-level BeforeAll behavior.**
- `[[=AfterEach{}]]` annotation: Runs once after every test
- `[[=AfterAll{}]]` annotation: Runs once after all of the tests. ***Important:** When running via CTest, each test is executed in a separate process, so AfterAll runs once per test executable (not once after all tests in the suite).**
- `[[=Parameterize{tuple(arg1, arg2), tuple(arg1, arg2)}]]` annotation: Parameterizes a test, runs it once with each set of arguments. The BeforeEach function is called before every parameterized run.
- `[[= Test{.disabled = true}]]` annotation: Disables a test, a quick alternative to deleting and then restoring later.
