/*!
 \defgroup LUA GenericContainer-Lua Interface
 \brief This module provides integration between the `GenericContainer` and Lua.

 The `GC_namespace` offers functionality to convert Lua global variables to and from
 a `GenericContainer` object, as well as execute Lua scripts using the `LuaInterpreter` class.

 ## Overview

 This tutorial explains how to use the Lua interface in the `GC_namespace` for working with
 Lua global variables and `GenericContainer` objects. It covers how to convert Lua global variables
 into a `GenericContainer` object and how to set Lua global variables from a `GenericContainer`.
 Additionally, the tutorial introduces the `LuaInterpreter` class for executing Lua code from C++.

 ## Working with Lua Global Variables and GenericContainer

 ### Converting Lua Global Variables to GenericContainer

 The function `Lua_global_to_GC` allows you to retrieve a Lua global variable and convert it into
 a `GenericContainer` object. This is useful when you want to access Lua variables in C++ and perform
 operations using `GenericContainer`'s versatile structure.

 \code
 void Lua_global_to_GC(
     void * void_L,
     string_view global_var,
     GenericContainer & gc
 );
 \endcode

 Example:
 \code
 GC_namespace::LuaInterpreter lua;
 lua.do_file("example.lua"); // Load a Lua script

 GenericContainer gc;
 lua.global_to_GC("someGlobalVar", gc); // Convert Lua global variable to GC

 std::cout << "Converted Lua global variable to GenericContainer: " << gc << std::endl;
 \endcode

 ### Converting GenericContainer to Lua Global Variables

 The function `Lua_GC_to_global` is the reverse operation of `Lua_global_to_GC`. It allows you to
 take a `GenericContainer` object and store its data in a Lua global variable. This is helpful when
 you need to pass C++ data structures into Lua.

 \code
 void Lua_GC_to_global(
     void * void_L,
     GenericContainer const & gc,
     string_view global_var
 );
 \endcode

 Example:
 \code
 GenericContainer gc;
 gc["key1"] = 42;
 gc["key2"] = "hello";

 GC_namespace::LuaInterpreter lua;
 lua.GC_to_global(gc, "luaGlobalVar"); // Pass GC data to Lua as a global variable

 lua.execute("print(luaGlobalVar.key1)"); // Outputs 42
 lua.execute("print(luaGlobalVar.key2)"); // Outputs "hello"
 \endcode

 ## Using the LuaInterpreter Class

 The `LuaInterpreter` class provides a simple interface for interacting with Lua scripts and
 commands from C++. It can execute Lua commands, call Lua functions, and work with Lua global
 variables and `GenericContainer`.

 ### Basic Usage of LuaInterpreter

 The `LuaInterpreter` class manages a Lua interpreter (`lua_State`) internally. You can create
 an instance of this class to load Lua scripts, execute Lua commands, and access Lua variables.

 Example of initializing the interpreter and running Lua commands:

 \code
 GC_namespace::LuaInterpreter lua;
 lua.execute("print('Hello from Lua')");
 \endcode

 ### Executing Lua Scripts and Commands

 The `LuaInterpreter::execute` function allows you to execute a Lua command string.
 This function is useful for executing arbitrary Lua code from your C++ application.

 \code
 void LuaInterpreter::execute( char const cmd[] );
 \endcode

 Example:
 \code
 GC_namespace::LuaInterpreter lua;
 lua.execute("x = 10");
 lua.execute("print(x)"); // Outputs 10
 \endcode

 ### Executing Lua Scripts from a File

 The `LuaInterpreter::do_file` function loads and executes a Lua script file. This is commonly
 used for configuration scripts or any Lua script file you want to run.

 \code
 void LuaInterpreter::do_file( char const fname[] );
 \endcode

 Example:
 \code
 GC_namespace::LuaInterpreter lua;
 lua.do_file("config.lua"); // Executes the Lua script in config.lua
 \endcode

 ### Calling Lua Functions from C++

 The `LuaInterpreter::call` function allows you to call a Lua function with arguments provided by
 a `GenericContainer`. The result of the Lua function is returned in another `GenericContainer`.

 The `arguments` container should have the following fields:
 - `"function"`: A string containing the name of the Lua function to be called.
 - `"args"`: A `GenericContainer` containing the function's arguments.

 \code
 void LuaInterpreter::call( GenericContainer const & arguments, GenericContainer & res );
 \endcode

 Example:
 \code
 GC_namespace::LuaInterpreter lua;
 lua.do_file("math_functions.lua"); // Contains Lua functions

 GenericContainer args;
 args["function"] = "add";
 args["args"].set_vec(2);
 args["args"][0] = 5;
 args["args"][1] = 3;

 GenericContainer result;
 lua.call(args, result);

 std::cout << "Result of Lua function add(5, 3): " << result.to_int() << std::endl;
 \endcode

*/
