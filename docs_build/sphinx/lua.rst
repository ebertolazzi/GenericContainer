Lua Support
===========

``GenericContainer`` has an interfacing to exchange data with Lua.
To use the interface include

.. code-block:: cpp

  #include "GenericContainerLuaInterface.hh"

compile and link with ``GenericContainerLuaInterface.cc``.
The interface contains a set of functions to convert from ``GenericContainer``
to Lua global variables and the other way around.
