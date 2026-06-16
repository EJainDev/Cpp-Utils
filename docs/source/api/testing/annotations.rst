Testing Annotations and Functions
=================================

.. cpp:namespace:: annotest

Enums
-----

.. cpp:enum-class:: OS

    Represents the different operating systems to enable or disable tests on specific platforms.

    .. cpp:enumerator:: Windows

        Runs only on Windows.

    .. cpp:enumerator:: Linux

        Runs only on Linux.

    .. cpp:enumerator:: Mac

        Runs only on macOS.

    .. cpp:enumerator:: Unknown

        Runs on any other platform.

Annotations
-----------

.. cpp:struct:: BeforeAll

    Annotates a function to be executed **once** before all tests in a test suite.

    .. note:: If this function throws an exception, the test suite is aborted.

    .. note:: Running with CTest causes this function to be executed before each test case, which may significantly slow down your tests.

.. cpp:struct:: BeforeEach

    Annotates a function to be executed before **each** test in a test suite.

    .. note:: If this function throws an exception, the test is aborted.

    .. important:: Only one function per suite may be annotated with ``BeforeEach``. If multiple are found, only the last one is used.

.. cpp:struct:: Test

    Annotates a function as a test case.

    .. important:: Tests must not use try-catch blocks around any ``assert`` or ``expect`` calls — use the assertion functions directly.

    The ``Test`` struct supports an optional string name and a ``disabled`` flag:

    * ``[[= Test{}]]`` — anonymous test, name derived from function name.
    * ``[[= Test<"My Named Test">{}]]`` — test with a custom display name.
    * ``[[= Test{.disabled = true}]]`` — skipped test.

.. cpp:struct:: AfterEach

    Annotates a function to be executed after **each** test in a test suite.

    .. note:: If this function throws an exception, only a warning is printed.

    .. important:: Only one function per suite may be annotated with ``AfterEach``.

.. cpp:struct:: AfterAll

    Annotates a function to be executed **once** after all tests in a test suite.

    .. note:: If this function throws an exception, only a warning is printed.

Utility Annotations
^^^^^^^^^^^^^^^^^^^

.. cpp:struct:: Disabled

    Annotates a test case to be disabled. The library prints that it is disabled and skips execution.

    .. deprecated:: Use the ``.disabled`` flag on ``Test`` instead (e.g., ``[[= Test{.disabled = true}]]``).

.. cpp:struct:: Parameterize

    Annotates a test case to run with different sets of parameters. Each parameter set is executed as a sub-test.

    To use, pass one or more :cpp:func:`tuple` calls, where each tuple represents a **complete** set of parameters for the function — each value corresponds to a function parameter in order.

    .. code-block:: cpp

        [[= Test{}, = Parameterize{tuple(5), tuple(50)} ]]
        void parameterized(int val) {
            assertTrue(val == 5 || val == 50);
        }

.. cpp:struct:: RequiresOS

    Annotates a test case to run only on specific operating systems.

    .. code-block:: cpp

        [[= Test{}, = RequiresOS{OS::Linux} ]] void testLinux();

    If the current OS does not match, the test is skipped with a diagnostic.

.. cpp:struct:: DisallowOS

    Annotates a test case to be skipped on specific operating systems.

    .. code-block:: cpp

        [[= Test{}, = DisallowOS{OS::Windows} ]] void test();

    Opposite of :cpp:struct:`RequiresOS`.

Functions
---------

.. cpp:function:: template <std::is_class T> int test(int argc, char** argv, T suite = {})

    The main entry point that runs all tests in a test suite and prints results.

    * ``argc, argv`` — forwarded from ``main()``; the library also recognizes ``--list`` (prints suite + test names) and ``--test-name <Name>`` (runs only that test).
    * ``suite`` — an optional instance of your test suite struct. By default the library value-initializes it.

    Returns **0** if all tests passed (or were skipped) and **1** if any test failed.