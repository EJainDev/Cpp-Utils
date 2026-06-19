.. _installation:

Installing
==========

Requirements
------------

To build AnnoTest, you need:

* **C++26 compatible compiler** — tested on GCC 16.1
* **CMake 4.3 or higher** — required for C++26 module support and `CXX_MODULES` file set support
    * The project uses the experimental C++23 `import std;` feature via the module feature UUID ``451f2fe2-a8a2-47c3-bc32-94786d8fc91b``
* **Ninja** — or another CMake generator that supports C++20 modules

Installation
------------

Clone and build:

.. code-block:: bash

    git clone https://github.com/EJainDev/AnnoTest.git && cd AnnoTest
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --target install

Advanced Installation
---------------------

* To build and run the self-tests, pass ``-DBUILD_TESTS=ON``

The library installs to your system prefix. To use it in another CMake project:

.. code-block:: cmake

    find_package(AnnoTest CONFIG REQUIRED)
    add_executable(my_tests main.cpp)
    target_link_libraries(my_tests PRIVATE annotest)

The installed CMake package also provides ``annotest_discover_tests()`` for CTest integration — see the API documentation.

Checkout how to get started with the library in the :ref:`Getting Started <getting_started>` section.
