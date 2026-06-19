export module annotest:death_test;

import :exceptions;
import :posix;

import std;

namespace annotest {
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

  // Note: The child inherits all file descriptors, signal handlers, and
  // atexit handlers from the parent. Avoid complex parent state when using
  // death tests — the child's lifetime is bounded by func()'s execution.
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
#endif

export void assertDeath(auto func) {
#if defined(__unix__) || defined(__APPLE__)
  auto result = runDeathTest(std::move(func));
  if (!result.died) {
    throw Error("Assertion failed: expected process death, child " + result.detail);
  }
#else
#warning "assertDeath not implemented for Windows"
#endif
}

export void expectDeath(auto func) {
#if defined(__unix__) || defined(__APPLE__)
  auto result = runDeathTest(std::move(func));
  if (!result.died) {
    throw Abort("Expectation failed: expected process death, child " + result.detail);
  }
#else
#warning "expectDeath not implemented for Windows"
#endif
}

}  // namespace annotest