.. _getting_started:

Getting Started
---------------

.. role:: cpp(code)
   :language: cpp

To get started with C++ Utils, first make sure you have installed the library as described in the :ref:`Installation <installation>` section of the documentation.

Currently, the library provides one major utility: the testing framework which you can checkout :ref:`here <testing_example>`.

.. _testing_example:

Testing Framework
^^^^^^^^^^^^^^^^^

You can begin using the testing framework with the following code:

.. _testing_example_code:

.. literalinclude:: ../../../examples/testing_example1.cpp
    :language: cpp
    :linenos:

The first important thing to notice is that it is NOT a header, instead you use :cpp:`import cpputils.testing` to use the framework. This also means that this framework is **MACRO FREE** 🎉.

In this example we see 3 major features:

1. The :cpp:struct:`~cpputils::testing::BeforeEach` annotation allows us to define a setup function that will run before each test case. This is useful for initializing common resources or state needed for the tests.
2. The :cpp:struct:`~cpputils::testing::Test` annotation is used to define test cases. Each function annotated with `Test` will be executed as a separate test case.
3. The :cpp:struct:`~cpputils::testing::Parameterize` annotation allows us to run the same test case with different sets of parameters. This is useful for testing a function with various inputs without having to write separate test cases for each input. *Note that the function does not take in std::tuple, it takes in a custom tuple type defined by the library*.

To actually run the test suite, you call the :cpp:func:`~cpputils::testing::test` function in your main function passing in the command line arguments and the type of your test suite struct. The library will handle running the tests and printing the results. You can choose to pass in a custom test suite instance as the last parameter, but by default, the library calls the default constructor.

Checkout the other features of the testing framework in the :ref:`API documentation <testing_api>`.