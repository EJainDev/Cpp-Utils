.. _getting_started:

Getting Started
---------------

.. role:: cpp(code)
   :language: cpp

To get started with AnnoTest, first make sure you have installed the library as described in the :ref:`Installation <installation>` section of the documentation.

Currently, the library provides one major utility: the testing framework.

.. _testing_example:

Testing Framework
^^^^^^^^^^^^^^^^^

You can begin using the testing framework with the following code:

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

The first important thing to notice is that it is NOT a header — instead you use ``import annotest;`` to use the framework. This also means that this framework is **MACRO FREE** 🎉.

In this example we see 4 major features:

1. The :cpp:struct:`~annotest::BeforeEach` annotation allows you to define a setup function that runs before each test case.
2. The :cpp:struct:`~annotest::Test` annotation defines test cases. Each function annotated with ``Test`` runs as a separate test.
3. The :cpp:struct:`~annotest::Parameterize` annotation lets you run the same test with different parameter sets. *Note that the function takes individual parameters — not a std::tuple — each value corresponds to a function parameter in order.*
4. The :cpp:func:`~annotest::test<T>` function in ``main`` runs the test suite, handling all annotations and printing results.