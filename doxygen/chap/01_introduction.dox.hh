/*!

 \page Introduction Introduction

## Brief description of the class and features

The `GenericContainer` class is a powerful and flexible data structure designed to manage a
wide variety of data types and structures.

Its capabilities include storing **integers**, **floating-point numbers**, **strings**, **vectors**, **matrices**, **maps**, and  and **complex numbers**.

This manual is a simple guide to using the `GenericContainer` class, providing explanations, usage examples,
and descriptions of various functionalities.

## Features

- **Data Types**:     Supports **integers**, **floating-point numbers**, **strings**, **vectors**, **matrices**,
                      **maps**, and **complex numbers**.
- **I/O**:            Allows reading and writing formatted data from and to files.
- **Interfacing**:    Read and save to **JSON**, **YAML** and **TOML** files.
                      SUpport for interfacing with **MATLAB**, **RUBY** and **PYTHON**.
- **Serialization**:  Facilitates serialization and deserialization of the entire container.
- **Error Handling**: Comprehensive error handling with exceptions and error codes.
- **Flexibility**:    Supports nested containers, enabling complex data structures.

## Class Overview

The `GenericContainer` class is designed
to be generic, allowing the user to store any data type.
It provides methods to manipulate the stored data and
to perform various operations, including:

- Setting and getting values.
- Managing collections of data with vectors and maps.
- Reading from and writing to files.
- Serializing and deserializing of the container.


`GenericContainer` is a `C++` class that permit to store eterogeneous
data:

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

\code{cpp}
GenericContainer gc;
gc["one"]  = 1;             // store integer
gc["two"]  = true;          // store a boolean
gc["3"]    = 1.4;           // store floating point number
gc["four"] = "pippo";       // store a string
gc["five"].set_vec_int(10); // store a vector of integer of 10 elements
\endcode

and to retrieve elements

\code{cpp}
cout << gc["one"].get_int()     << '\n';
cout << gc["two"].get_bool()    << '\n';
cout << gc["3"].get_real()      << '\n';
cout << gc["four"].get_string() << '\n';
GenericContainer::vec_int_type & v = gc["five"].get_vec_int();
cout << v[1] << '\n';
\endcode

For more complex emxamples and recursive data see example test files in
the distribution or the \ref Usage "Usage" section of the manual.

*/
