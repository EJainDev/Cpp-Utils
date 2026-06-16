.. _death_tests:

Death Tests
===========

.. cpp:namespace:: annotest

Death tests verify that code under test causes the process to exit abnormally. They use ``fork()`` / ``waitpid()`` on Unix and macOS to isolate the child process — the parent never crashes.

.. note:: Death tests are not available on Windows.

See also :cpp:func:`annotest::assertDeath` and :cpp:func:`annotest::expectDeath`.

Under the Hood
--------------

The ``fork()`` call creates a child process that runs ``func()`` in a try-catch block. The parent waits with ``waitpid()`` and checks the exit status. A normal exit (status 0) is treated as a failure; any non-zero exit is treated as a successful death.
