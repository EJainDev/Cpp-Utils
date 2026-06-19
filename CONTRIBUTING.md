# Contributing to AnnoTest

Thank you for your interest in contributing to AnnoTest!

## Getting Started

1. **Fork** the repository
2. **Clone** your fork locally
3. **Create a branch** for your feature or fix
4. **Write tests** for new functionality
5. **Submit a pull request** with a clear description

## Development Setup

You'll need:
- GCC 16.1+ (C++26 support required)
- CMake 4.3+ (required for CXX_MODULES file set and `import std;` support)
- Ninja build system

```bash
# Build the library and tests
cmake -B build -DCMAKE_C_COMPILER=gcc-16 -DCMAKE_CXX_COMPILER=g++-16 \
       -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build/tests
```

## Coding Standards

- Follow the existing Google style (100 columns)
- All code must be in the `annotest::` namespace
- Use C++26 module syntax (`.cxx` files with `export module`)
- Follow the existing annotation-based API design
- Add regression tests for bug fixes

## Commit Messages

Use [conventional commits](https://www.conventionalcommits.org/) format:
- `feat:` New feature
- `fix:` Bug fix
- `docs:` Documentation changes
- `test:` Test additions or changes
- `refactor:` Code refactoring
- `chore:` Maintenance tasks

## Pull Request Guidelines

1. **Scope**: Each PR should address a single concern
2. **Tests**: All new functionality needs tests
3. **Documentation**: Update relevant documentation
4. **Build**: Ensure `cmake --build build` succeeds
5. **Tests**: Ensure all tests pass via `ctest`

## Bug Reports

When filing a bug report, include:
- Compiler version and C++ standard
- Minimal reproduction case
- Expected vs actual behavior
- Relevant stack traces (if any)

## Feature Requests

Before proposing new features:
1. Check existing issues for similar requests
2. Explain the use case clearly
3. Consider compatibility with existing code
