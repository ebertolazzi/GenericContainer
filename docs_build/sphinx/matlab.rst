Matlab Support
==============

``GenericContainer`` has an interfacing to exchange data with matlab.
To use the interface include

.. code-block:: cpp

  #include "GenericContainerMatlabInterface.hh"

compile and link with ``GenericContainerMatlabInterface.cc``.
The interface contains a set of functions to convert from ``GenericContainer``
to ``mxArray`` and the other way around.

The following code stored in ``mex_print_recursive.cc``

.. code-block:: cpp

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

Implement a mex command that print a Matlab structure
recursively on the console after the conversion to a
``mxArray_to_GenericContainer``.
After the compilation

.. code-block:: text

  > mex mex_print_recursive.cc GenericContainerMatlabInterface.cc -output print_recursive

Produce the ``Matlab`` command ``print_recursive``
so that the following MATLAB code:

.. code-block:: matlab

  S         = [ 1 0 2 9; 0 0 2 3; 2 0 0 0; 1 0 -2 -2 ];
  S1        = [ 1 0 2 9; 0 0 2 3; 2+1i 0 0 0; 1 0 -2 -2 ];
  A.vector  = [1,2,3,4];
  A.string  = 'pippo';
  A.strings = { 'pippo', 'pluto', 'paperino' };
  A.struct1  = { 'paperino', [1 2], [1 2; 3 5] };
  A.struct2  = { B, sparse(S), sparse(S1) };

  print_recursive( A );

has the following output:

.. code-block:: text

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
