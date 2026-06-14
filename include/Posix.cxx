module;

#if defined(__unix__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

export module annotest:posix;

namespace annotest {
#if defined(__unix__) || defined(__APPLE__)
using pid_t = __pid_t;

__pid_t fork() { return ::fork(); }

__pid_t waitpid(__pid_t pid, int* status, int options) { return ::waitpid(pid, status, options); }
#endif
}  // namespace annotest