.. _death_tests:

Death Tests
===========

.. cpp:namespace:: annotest

Death tests verify that code under test causes the process to exit abnormally. They use ``fork()`` / ``waitpid()`` on Unix and macOS to isolate the child process — the parent never crashes.

.. note:: Death tests are not available on Windows.

Under the Hood
--------------

The ``fork()`` call creates a child process that runs ``func()`` in a try-catch block. The parent waits with ``waitpid()`` and checks the exit status. A normal exit (status 0) is treated as a failure; any non-zero exit is treated as a successful death.

Example
-------

.. code-block:: cpp

   [[= Test{"death by exception"}]]
   void dies_on_exception() {
       assertDeath([]() { throw std::runtime_error("crash!"); });
   }

   [[= Test{"death by abort"}]]
   void dies_on_abort() {
       assertDeath([]() { std::abort(); });
   }

For full function signatures, see :doc:`assertions`.
