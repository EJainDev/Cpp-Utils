Testing Annotations and Functions
=================================

.. cpp:namespace:: annotest

This page documents all annotations, enums, and the test runner function. For assertion and expectation APIs, see :doc:`assertions`.

.. _design_decisions:

.. _example_gallery:

Design Decisions
----------------

AnnoTest makes deliberate design choices that distinguish it from other C++ testing frameworks. Read :ref:`design_decisions` in Getting Started for full details.

.. important:: Test suite structs must be **public aggregates** with default member initializers. Do not use constructors, private members, or virtual functions.

Enums
-----

.. cpp:enum-class:: OS

    Represents the different operating systems to enable or disable tests on specific platforms.

    .. cpp:enumerator:: Windows

        Windows (Win32).

    .. cpp:enumerator:: Mac

        macOS (Apple).

    .. cpp:enumerator:: Linux

        Linux.

    .. cpp:enumerator:: Unknown

        Any other platform.

Annotations
-----------

.. cpp:struct:: Test

    Annotates a function as a test case. The function name is used as the test display name unless a custom name is provided.

    The ``Test`` struct supports an optional string name and a ``disabled`` flag:

    * ``[[= Test{}]]`` — anonymous test, name derived from function name.
    * ``[[= Test<\"My Named Test\">{}]]`` — test with a custom display name.
    * ``[[= Test{.disabled = true}]]`` — skipped test.

    .. code-block:: cpp

        struct MyTests {
            [[= Test{}]] void anonymous_test() { assertEqual(1, 1); }

            [[= Test<"custom name">{}]] void actual_name() { assertTrue(true); }

            [[= Test{.disabled = true}]] void skipped() {}
        };

    .. important:: Tests must not use try-catch blocks around any ``assert`` or ``expect`` calls — use the assertion functions directly.

    See also :ref:`basic example <example_gallery>`.

.. cpp:struct:: BeforeEach

    Annotates a function to be executed before **each** test in a test suite.

    .. note:: If this function throws an exception, the test is aborted.

    .. important:: Only one function per suite may be annotated with ``BeforeEach``. If multiple are found, only the last one is used.

    .. code-block:: cpp

        struct FixtureTests {
            int counter = 0;

            [[= BeforeEach{}]] void reset() { counter = 0; }

            [[= Test{}]] void one() { ++counter; assertTrue(counter == 1); }
            [[= Test{}]] void two() { ++counter; ++counter; assertTrue(counter == 2); }
        };

    See also :ref:`lifecycle example <example_gallery>`.

.. cpp:struct:: AfterEach

    Annotates a function to be executed after **each** test in a test suite.

    .. note:: If this function throws an exception, only a warning is printed.

    .. important:: Only one function per suite may be annotated with ``AfterEach``.

    See also :ref:`lifecycle example <example_gallery>`.

.. cpp:struct:: BeforeAll

    Annotates a function to be executed **once** before all tests in a test suite.

    .. note:: If this function throws an exception, the test suite is aborted.

    .. note:: Running with CTest causes this function to be executed before each test case, which may significantly slow down your tests.

    See also :ref:`lifecycle example <example_gallery>`.

.. cpp:struct:: AfterAll

    Annotates a function to be executed **once** after all tests in a test suite.

    .. note:: If this function throws an exception, only a warning is printed.

    See also :ref:`lifecycle example <example_gallery>`.

Utility Annotations
^^^^^^^^^^^^^^^^^^^

.. cpp:struct:: Parameterize

    Annotates a test case to run with different sets of parameters. Each parameter set is executed as a sub-test.

    To use, pass one or more :cpp:func:`tuple` calls, where each tuple represents a **complete** set of parameters for the function — each value corresponds to a function parameter in order.

    .. code-block:: cpp

        [[= Test{}, = Parameterize{tuple(5), tuple(50)} ]]
        void parameterized(int val) {
            assertTrue(val == 5 || val == 50);
        }

    With multiple parameters:

    .. code-block:: cpp

        [[= Test{}, = Parameterize{tuple(2, 3), tuple(10, 20)} ]]
        void add(int a, int b) {
            assertEqual(a + b, a + b);  // parameters injected in order
        }

    See also :ref:`parameterized example <example_gallery>`.

.. cpp:struct:: ParameterizeTemplate

    Similar to :cpp:struct:`Parameterize`, but passes **typed** tuples to the test function. Use this when you want explicit type safety for complex parameter packs.

    .. code-block:: cpp

        [[= Test{}, = ParameterizeTemplate{tuple(10), tuple(20)} ]]
        void multiples(int n) {
            assertTrue(n % 10 == 0);
        }

.. cpp:struct:: RequiresOS

    Annotates a test case to run only on specific operating systems.

    .. code-block:: cpp

        [[= Test{}, = RequiresOS{OS::Linux} ]] void testLinux();

    If the current OS does not match, the test is skipped with a diagnostic.

    Accepts multiple OS values — the test runs if the current OS matches **any** of them:

    .. code-block:: cpp

        [[= Test{}, = RequiresOS{OS::Linux, OS::Mac} ]] void cross_platform();

    See also :ref:`OS annotations example <example_gallery>`.

.. cpp:struct:: DisallowOS

    Annotates a test case to be skipped on specific operating systems.

    .. code-block:: cpp

        [[= Test{}, = DisallowOS{OS::Windows} ]] void test();

    Opposite of :cpp:struct:`RequiresOS`. The test runs on any platform **not** listed.

    Multiple values — the test is skipped if the current OS matches **any** of them:

    .. code-block:: cpp

        [[= Test{}, = DisallowOS{OS::Windows, OS::Mac} ]] void linux_only();

    See also :ref:`OS annotations example <example_gallery>`.

Functions
---------

.. cpp:function:: template <std::is_class T> int test(int argc, char** argv, T suite = {})

    The main entry point that runs all tests in a test suite and prints results.

    * ``argc, argv`` — forwarded from ``main()``; the library also recognizes ``--list`` (prints suite + test names) and ``--test-name <Name>`` (runs only that test).
    * ``suite`` — an optional instance of your test suite struct. By default the library value-initializes it.

    Returns **0** if all tests passed (or were skipped) and **1** if any test failed.

    .. code-block:: cpp

        int main(int argc, char** argv) {
            return test<MyTests>(argc, argv);
        }

.. cpp:function:: template <typename... Ts> constexpr auto tuple(Ts... args)

    Creates a compile-time tuple for use with :cpp:struct:`Parameterize` or :cpp:struct:`ParameterizeTemplate`.

    .. code-block:: cpp

        auto t = tuple(5, 10);    // Tuple<int, int>
        auto u = tuple(3.14);     // Tuple<double>

    See also :ref:`parameterized example <example_gallery>`.

.. cpp:function:: template <typename E> consteval auto getMembers()

    Returns the compile-time reflection members of a type. Used internally for test discovery.

.. cpp:function:: template <typename E> consteval auto getAnnotations()

    Returns the compile-time annotations on a reflection entity.

.. cpp:function:: template <typename E> consteval auto getNonstaticDataMembers()

    Returns the compile-time non-static data members of a type.
