.. _testing_api:

Testing Library
===============

The testing library provides a C++26 annotation-based framework for writing unit tests. It uses C++26 reflection at compile time to discover test functions and setup/teardown hooks — no macros needed.

The design is inspired by Python's *unittest* and Java's *JUnit*, making it familiar for users coming from those ecosystems.

.. toctree::
    :maxdepth: 2

    annotations
    assertions
    contracts
    death