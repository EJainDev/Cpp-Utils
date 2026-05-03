module;

#if defined(__unix__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

export module cpputils.testing:death_test;

import :exceptions;

import std;

namespace cpputils::testing {
struct DeathResult {
  bool died;
  std::string detail;
};

#if defined(__unix__) || defined(__APPLE__)
template <typename F>
DeathResult runDeathTest(F&& func) {
  const pid_t pid = fork();

  if (pid < 0) {
    return {false, "fork() failed"};
  }

  // It is the child if pid == 0
  if (pid == 0) {
    try {
      func();
      std::_Exit(0);
    } catch (...) {
      std::_Exit(1);
    }
  }

  int status = 0;
  if (waitpid(pid, &status, 0) < 0) {
    return {false, "waitpid() failed"};
  }

  if (status == 0) {
    return {false, "exited normally with status 0"};
  }
  return {true, std::format("exited with status {}", status)};
}

export void assertDeath(auto func) {
  auto result = runDeathTest(std::move(func));
  if (!result.died) {
    throw Error("Assertion failed: expected process death, child " + result.detail);
  }
}

export void expectDeath(auto func) {
  auto result = runDeathTest(std::move(func));
  if (!result.died) {
    throw Abort("Expectation failed: expected process death, child " + result.detail);
  }
}
#endif

}  // namespace cpputils::testing