.. _installation:

Installing
==========

.. role:: cpp(code)
   :language: cpp

Requirements
------------

To install C++ Utils, you need the following:

* A C++26 compatible compiler (this library has been tested on GCC 16.1)
* CMake 4.2.3 or higher (this library has been tested on CMake 4.2.3)
    * It is unlikely that higher or lower versions will work because of the experimental flag for C++23 :cpp:`import std;` If you find a solution for another version, please create a PR on `GitHub <https://www.github.com/EJainDev/Cpp-Utils>`__.
* Ninja or other CMake generators that support C++20 modules


Installation
------------

Run the following commands to install the library:

.. code-block:: bash

    git clone https://www.github.com/EJainDev/Cpp-Utils.git && cd Cpp-Utils
    
    mkdir build

    cmake -B build -S . -D CMAKE_BUILD_TYPE=Release
    cmake --build build --target install --config Release


Advanced Installation
---------------------

To install and build the tests, pass the *-D ENABLE_TESTING=ON* flag to CMake.

To enable code coverage reports, pass the *-D ENABLE_COVERAGE=ON* flag to CMake and make sure that testing is enabled.


Checkout how to get started with the library in the :ref:`Getting Started <getting_started>` section of the documentation.