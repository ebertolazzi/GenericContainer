Generic Container
=================

``GenericContainer`` is a C++ class that permit to store eterogeneous
data:

.. list-table:: Supported types
  :widths: 10 10 10

  * -  pointer
    -  boolean
    -  integer
  * -  long integer
    -  floating point
    -  complex floating point
  * -  string
    -  vector of pointer
    -  vector of boolean
  * -  vector of integer
    -  vector of floating point
    -  vector of complex floating point
  * -  vector of string
    -  matrix of floating point
    -  matrix of complex floating point

in addition to this data type the following two container data are avaiable

-  vector of ``GenericContainer``
-  map of ``GenericContainer``

this permits to build complex recursive data. The main usage of the
class is in interchange data with scripting language like ``Ruby``,
``Lua``, ``MATLAB``.

Simple Usage
------------

The usage is simple, for example it can be used as an associative array
with eterogenous data

.. code:: cpp

   GenericContainer gc;
   gc["one"]  = 1;             // store integer
   gc["two"]  = true;          // store a boolean
   gc["3"]    = 1.4;           // store floating point number
   gc["four"] = "pippo";       // store a string
   gc["five"].set_vec_int(10); // store a vector of integer of 10 elements

and to retrieve elements

.. code:: cpp

   cout << gc["one"].get_int()     << '\n';
   cout << gc["two"].get_bool()    << '\n';
   cout << gc["3"].get_real()      << '\n';
   cout << gc["four"].get_string() << '\n';
   GenericContainer::vec_int_type & v = gc["five"].get_vec_int();
   cout << v[1] << '\n';

For more complex emxamples and recursive data see example test files in
the distribution.

Compile and tests
-----------------

Edit makefile file to match compiler of your OS and do:

.. code:: sh

     make # using make (only linux/OSX or mingw)

     rake build_osx   # compile for MAC
     rake build_linux # compile for linux
     rake build_win   # compile for OSX

To run the test

.. code:: sh

     make run

     rake run       # for linux/OSX
     rake run_win   # for windows

Contents
--------

.. toctree::
   :maxdepth: 2

   api-cpp/root.rst
   api-c/root.rst
   api-json/root.rst
   matlab.rst
   lua.rst

.. include:: author.rst

License
~~~~~~~

.. literalinclude:: ../../license.txt
    :language: none
