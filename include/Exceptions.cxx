export module annotest:exceptions;

import std;

namespace annotest {
// An exception class for test failures, to distinguish from other exceptions. It does not inherit
// std::exception to prevent the user from catching it as frequently
class Error {
 public:
  explicit Error(std::string message) : message_(std::move(message)) {}

  const std::string& message() const { return message_; }

 private:
  std::string message_;
};

// An exception class for abort messages that should not be treated as test failures (e.g. failed
// setup)
class Abort {
 public:
  explicit Abort(std::string message) : message_(std::move(message)) {}

  const std::string& message() const { return message_; }

 private:
  std::string message_;
};
}  // namespace annotest