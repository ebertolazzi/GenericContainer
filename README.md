# Generic Container

`GenericContainer` is a `C++` class with permit to store eterogeneous
data:


- [Online documentation](http://ebertolazzi.github.io/GenericContainer)

## Supported types

- pointer
- boolean
- integer
- long integer
- floating point
- complex floating point
- string
- vector of pointer
- vector of boolean
- vector of integer
- vector of floating point
- vector of complex floating point
- vector of string
- matrix of floating point
- matrix of complex floating point

in addition to this data type the following two container data are avaiable

-  vector of `GenericContainer`
-  map of `GenericContainer`

this permits to build complex recursive data. The main usage of the
class is in interchange data with scripting language like `Ruby`,
`Lua`, `MATLAB`.

## Simple Usage

The usage is simple, for example it can be used as an associative array
with eterogenous data

```cpp
GenericContainer gc;
gc["one"]  = 1;             // store integer
gc["two"]  = true;          // store a boolean
gc["3"]    = 1.4;           // store floating point number
gc["four"] = "pippo";       // store a string
gc["five"].set_vec_int(10); // store a vector of integer of 10 elements
```

and to retrieve elements

``` cpp
cout << gc["one"].get_int()     << '\n';
cout << gc["two"].get_bool()    << '\n';
cout << gc["3"].get_real()      << '\n';
cout << gc["four"].get_string() << '\n';
GenericContainer::vec_int_type & v = gc["five"].get_vec_int();
cout << v[1] << '\n';
```

For more complex emxamples and recursive data see example test files in
the distribution.

## Compile and tests

Tou need rake installed in your system

```sh
rake
```

if the OS is not correctly detected try

```sh
rake build_osx   # compile for OSX
rake build_linux # compile for linux
rake build_win   # compile for WINDOWS
```

To run the test

```sh
rake run       # for linux/OSX
rake run_win   # for windows
```

## Lua Support

`GenericContainer` has an interfacing to exchange data with Lua.
To use the interface include

```cpp
#include "GenericContainerInterface_lua.hh"
```

compile and link with `GenericContainerInterface_lua.cc`.
The interface contains a set of functions to convert from `GenericContainer`
to Lua global variables and the other way around.

## Matlab Support

`GenericContainer` has an interfacing to exchange data with matlab.
To use the interface include

```cpp
#include "GenericContainerInterface_matlab.hh"
```

compile and link with `GenericContainerMatlabInterface.cc`.
The interface contains a set of functions to convert from `GenericContainer`
to `mxArray` and the other way around.

The following code stored in `mex_print_recursive.cc`

```cpp
#include "GenericContainer.hh"
#include "GenericContainerMatlabInterface.hh"
#include "mex.h"

#include <sstream>

namespace GC_namespace {

  extern "C"
  void
  mexFunction( int nlhs, mxArray       *plhs[],
               int nrhs, mxArray const *prhs[] ) {
    try {
      GenericContainer gc;
      mxArray_to_GenericContainer( prhs[0], gc );
      mexPrint(gc);
    }
    catch ( std::exception & exc ) {
      mexPrintf("Error: %s\n", exc.what() );
    }
    catch (...) {
      mexPrintf("Unknown error\n");
    }
  }
}
```

Implement a mex command that print a Matlab structure
recursively on the console after the conversion to a
`mxArray_to_GenericContainer`.
After the compilation

```text
  > mex mex_print_recursive.cc GenericContainerMatlabInterface.cc -output print_recursive
```

Produce the `Matlab` command `print_recursive`
so that the following MATLAB code:

```matlab
S         = [ 1 0 2 9; 0 0 2 3; 2 0 0 0; 1 0 -2 -2 ];
S1        = [ 1 0 2 9; 0 0 2 3; 2+1i 0 0 0; 1 0 -2 -2 ];
A.vector  = [1,2,3,4];
A.string  = 'pippo';
A.strings = { 'pippo', 'pluto', 'paperino' };
A.struct1  = { 'paperino', [1 2], [1 2; 3 5] };
A.struct2  = { B, sparse(S), sparse(S1) };

print_recursive( A );
```

has the following output:

```text

  string: "pippo"
  strings:
      0: "pippo"
      1: "pluto"
      2: "paperino"
  struct1:
      0: "paperino"
      1: [ 1 2 ]
      2:
         1        2
         3        5
  struct2:
      0:
          fieldA:
              0: 1
              1: 2
              2: 3
              3: "pippo"
          fieldB:
              [ 1 1 2 ]
          fieldC: "stringa"
      1:
          ir:
              [ 0 2 3 0 1 3 0 1 3 ]
          jc:
              [ 0 3 3 6 9 ]
          values:
              [ 1 2 1 2 2 -2 9 3 -2 ]
      2:
          ir:
              [ 0 2 3 0 1 3 0 1 3 ]
          jc:
              [ 0 3 3 6 9 ]
          values:
              [ (1, 0 ) (2, 1 ) (1, 0 ) (2, 0 ) (2, 0 ) (-2, 0 ) (9, 0 ) (3, 0 ) (-2, 0 ) ]
  vector:
      [ 1 2 3 4 ]
```
