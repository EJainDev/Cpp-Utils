.. _getting_started:

Getting Started
---------------

.. _testing_example:

Testing Framework
^^^^^^^^^^^^^^^^^

.. code-block:: cpp

    import annotest;

    struct AddTest {
        [[= Test{}]] void basic() {
            assertEqual(4, 2 + 2);
        }

        [[= Test{.disabled = true}]] void skipped() {}

        [[= Test{}, = Parameterize{tuple(5), tuple(50)} ]] void parameterized(int val) {
            assertTrue(val == 5 || val == 50);
        }
    };

    int main(int argc, char** argv) {
        return test<AddTest>(argc, argv);
    }

Using ``import annotest;``
~~~~~~~~~~~~~~~~~~~~~~~~~~

AnnoTest is a **C++26 module**, not a header-only library. You never write ``#include <annotest.hpp>``. Instead, you write:

.. code-block:: cpp

    import annotest;

This imports the umbrella module, which re-exports all sub-modules: ``annotest:asserts``, ``annotest:expects``, ``annotest:contracts``, ``annotest:exceptions``, ``annotest:death_test``, ``annotest:tuple``, ``annotest:utils``, and ``annotest:posix``.

This is **MACRO FREE** 🎉. No preprocessor pollution, no include guards, no translation-unit duplication. The module system gives you a single import point with zero overhead.

.. _design_decisions:

Design Decisions
^^^^^^^^^^^^^^^^

AnnoTest makes deliberate design choices that distinguish it from other C++ testing frameworks.

Why structs, not classes?
~~~~~~~~~~~~~~~~~~~~~~~~~

All test suite structs use **public members** by design:

.. code-block:: cpp

    struct AddTest {
        int counter = 0;  // public — you can read and write directly

        [[= BeforeEach{}]] void reset() { counter = 0; }

        [[= Test{}]] void one() {
            ++counter;  // direct access, no getters/setters
            assertTrue(counter == 1);
        }
    };

**Why?** AnnoTest discovers test members through C++26 reflection (`std::meta::members_of`), which only sees **non-private** members. By using plain structs with public data members, you get:

1. **Zero boilerplate** — no getters, no setters, no friend declarations
2. **Direct state sharing** between ``BeforeEach``, ``AfterEach``, and test methods
3. **Reflection compatibility** — the library can see all your members without intrusive changes
4. **No inheritance needed** — composition through struct members is sufficient for test fixtures

**You still get encapsulation if you want it** — AnnoTest doesn't prevent you from using private members. It simply doesn't require or use them.

Plain old structs (POD)
~~~~~~~~~~~~~~~~~~~~~~~

AnnoTest expects your test suite struct to be **value-initializable** with no constructor logic. The library creates an instance of your struct internally:

.. code-block:: cpp

    T suite = {};  // value-initialization, no arguments

This means:

* Use default member initializers, not constructors
* Keep fixture state simple — plain ints, vectors, strings
* No virtual functions needed (AnnoTest doesn't use polymorphism)

No inheritance
~~~~~~~~~~~~~~

Test suites are **not base classes** — you don't extend them. Each ``struct MyTest`` is a standalone unit that gets discovered and run independently. If you need shared state, put it in the struct itself or use ``static`` members.

Data members go in the struct
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Put your test fixtures **inside** the struct, not in ``main()`` or globals:

.. code-block:: cpp

    // Good — fixture lives with its tests
    struct AddTest {
        int x = 10;
        int y = 20;

        [[= BeforeEach{}]] void reset() { x = 0; y = 0; }

        [[= Test{}]] void sum() { assertEqual(30, x + y); }
    };

.. code-block:: cpp

    // Bad — state separated from tests
    int x = 10;
    int y = 20;

    struct AddTest {
        [[= BeforeEach{}]] void reset() { x = 0; y = 0; }  // hidden globals
    };

No private members
~~~~~~~~~~~~~~~~~~

Don't use private access specifiers in your test structs. AnnoTest's reflection discovery reads all members at compile time and needs them to be visible. Private members are invisible to ``std::meta::members_of`` and will be silently ignored by the framework.

Default members only
~~~~~~~~~~~~~~~~~~~~

All struct members should have default values. The library value-initializes your struct:

.. code-block:: cpp

    // Good — all members have defaults
    struct MyTest {
        int counter = 0;
        std::vector<int> data{};
        std::string name = "test";

        [[= BeforeEach{}]] void reset() { counter = 0; data.clear(); }
    };

    // Bad — no defaults, library can't construct
    struct BadTest {
        int counter;  // uninitialized — undefined behavior
        std::string name;  // empty, but no explicit default

        [[= BeforeEach{}]] void reset() { counter = 0; }
    };

No constructors
~~~~~~~~~~~~~~~

The library creates your struct with ``T{}`` (value-initialization). Do not define constructors:

.. code-block:: cpp

    // Bad — AnnoTest can't call this
    struct BadTest {
        BadTest() { /* setup */ }
    };

Use ``BeforeEach`` for setup logic instead:

.. code-block:: cpp

    // Good
    struct GoodTest {
        [[= BeforeEach{}]] void setup() { /* setup logic */ }
    };

Aggregate initialization
~~~~~~~~~~~~~~~~~~~~~~~~

Your test suite struct is an **aggregate** — it can be initialized with brace-enclosed initializer lists. The library uses this for test discovery and member iteration.

More Examples
~~~~~~~~~~~~~

See the complete example gallery in :ref:`Getting Started <testing_example>`:

* :doc:`/examples/basic` — basic tests, named tests, disabled tests
* :doc:`/examples/lifecycle` — BeforeAll, BeforeEach, AfterEach, AfterAll hooks
* :doc:`/examples/parameterized` — Parameterize and ParameterizeTemplate with tuple
* :doc:`/examples/os_annotations` — RequiresOS and DisallowOS for platform-specific tests
* :doc:`/examples/death_tests` — assertDeath and expectDeath for crash-testing
* :doc:`/examples/contracts` — C++26 contract testing with assertContractViolation
* :doc:`/examples/full_suite` — comprehensive demo combining ALL features

.. note:: This library is still under development. Breaking changes may occur.
