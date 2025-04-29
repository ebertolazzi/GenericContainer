/*!

 \mainpage GenericContainer

 \author Enrico Bertolazzi

 - Dipartimento di Ingegneria Industriale
 - Università degli Studi di Trento
 - [personal homepage](https://e.bertolazzi.dii.unitn.it)

 \copyright [The 2-Clause BSD License](https://opensource.org/license/bsd-2-clause)


 - [Github repository](https://github.com/ebertolazzi/GenericContainer)
 - \subpage Install
 - \subpage Usage
 - \subpage License

## Introduction

The `GenericContainer` class is a powerful and
flexible data structure designed to manage a
wide variety of data types and structures.
Its capabilities include storing integers, real numbers,
strings, vectors, maps, and more.
This manual is a simple guide to using the `GenericContainer` class,
providing explanations, usage examples,
and descriptions of various functionalities.

## Features

- **Data Types**: Supports integers, floating-point numbers, strings, vectors, maps, and complex numbers.
- **Formatted I/O**: Allows reading and writing formatted data from and to files.
- **Serialization**: Facilitates serialization and deserialization of the entire container.
- **Error Handling**: Comprehensive error handling with exceptions and error codes.
- **Flexibility**: Supports nested containers, enabling complex data structures.

## Class Overview

The `GenericContainer` class is designed
to be generic, allowing the user to store any data type.
It provides methods to manipulate the stored data and
to perform various operations, including:

- Setting and getting values.
- Managing collections of data with vectors and maps.
- Reading from and writing to files.
- Serializing and deserializing the container.

## Example Usage

This section presents various examples demonstrating
the functionalities of the `GenericContainer` class.

### Example 1: Basic Operations

This example illustrates basic operations,
including setting and retrieving integer values.

\code{.cc}
#include "GenericContainer.hh"
#include <iostream>

using namespace std;

int main() {
    GenericContainer gc;

    // Setting an integer value
    gc["integerValue"] = 42;

    // Retrieving the integer value
    int value = gc["integerValue"].get_int();
    cout << "The integer value is: " << value << endl;

    return 0;
}
\endcode

### Example 2: Working with Vectors

This example shows how to create and
manipulate a vector within a `GenericContainer`.

\code{.cc}
#include "GenericContainer.hh"
#include <iostream>

using namespace std;

int main() {
    GenericContainer gc;
    gc.set_vec_real(5);  // Create a vector of 5 real numbers

    // Assigning values to the vector
    gc[0] = 3.14;
    gc[1] = 2.71;
    gc[2] = 1.41;

    // Displaying vector contents
    cout << "Vector contents: ";
    for (int i = 0; i < gc.get_vector_size(); i++) {
        cout << gc[i].get_real() << " ";
    }
    cout << endl;

    return 0;
}
\endcode

### Example 3: Using Maps

Demonstrates how to create and manipulate
maps within a `GenericContainer`.

\code{.cc}
#include "GenericContainer.hh"
#include <iostream>

using namespace std;

int main() {
    GenericContainer gc;
    gc.set_map();  // Create a map

    // Setting values in the map
    gc["key1"] = 123;
    gc["key2"] = "example";

    // Retrieving values from the map
    cout << "Key1: " << gc["key1"].get_int() << endl;
    cout << "Key2: " << gc["key2"].get_string() << endl;

    return 0;
}
\endcode

### Example 4: Nested Containers

Illustrates how to create nested containers with vectors and maps.

\code{.cc}
#include "GenericContainer.hh"
#include <iostream>

using namespace std;

int main() {
    GenericContainer gc;
    gc.set_vec_real(2);  // Create a vector of 2 elements

    // Create first element as a map
    gc[0].set_map();
    gc[0]["innerKey1"] = 100;

    // Create second element as another vector
    gc[1].set_vec_real(3);
    gc[1][0] = 1.0;
    gc[1][1] = 2.0;
    gc[1][2] = 3.0;

    // Display contents
    cout << "Inner Key1: " << gc[0]["innerKey1"].get_int() << endl;
    cout << "Vector contents of second element: ";
    for (int i = 0; i < gc[1].get_vector_size(); i++) {
        cout << gc[1][i].get_real() << " ";
    }
    cout << endl;

    return 0;
}
\endcode

### Example 5: Serialization

This example showcases how to serialize and deserialize a `GenericContainer`.

\code{.cc}
#include "GenericContainer.hh"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    GenericContainer gc;
    gc["name"] = "Doxygen";
    gc["version"] = 1.8;

    // Serialize the GenericContainer
    int sz = gc.mem_size();
    vector<uint8_t> buffer( static_cast<size_t>( sz ) );
    int sz1 = gc.serialize(sz, buffer.data());

    cout << "Serialized size: " << sz1 << endl;

    // Deserialize into a new GenericContainer
    GenericContainer gc_new;
    gc_new.de_serialize(sz, buffer.data());

    cout << "Deserialized Name: " << gc_new["name"].get_string() << endl;

    return 0;
}
\endcode

### Example 6: Complex Data Structures

Demonstrates the creation of a complex
data structure with various data types.

\code{.cc}
#include "GenericContainer.hh"
#include <iostream>

using namespace std;

int main() {
    GenericContainer gc;

    // Create a map with different data types
    gc.set_map();
    gc["intValue"] = 100;
    gc["stringValue"] = "Hello World";
    gc["realValue"] = 3.14159;

    // Accessing values
    cout << "Integer: " << gc["intValue"].get_int() << endl;
    cout << "String: " << gc["stringValue"].get_string() << endl;
    cout << "Real: " << gc["realValue"].get_real() << endl;

    return 0;
}
\endcode

### Example 7: File I/O

This example demonstrates the use of file I/O with `GenericContainer` to read and print formatted data.

\code{.cc}
#include "GenericContainer.hh"
#include <iostream>
#include <fstream>

using namespace std;

int main() {
    GenericContainer gc;
    ifstream file("data_example.txt");

    if (file.fail()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    gc.readFormattedData(file, "#", "\t ");
    gc.dump(cout);  // Print contents

    return 0;
}
\endcode

### Example 8: Matrix Operations

This example shows how to create and manipulate a matrix using `GenericContainer`.

\code{.cc}
#include "GenericContainer.hh"
#include <iostream>

using namespace std;

int main() {
    GenericContainer gc;
    gc.set_mat_real(3, 3);  // Create a 3x3 matrix

    // Assigning values to the matrix
    gc.get_real_at(0, 0) = 1.0;
    gc.get_real_at(1, 1) = 2.0;
    gc.get_real_at(2, 2) = 3.0;

    // Display matrix contents
    cout << "Matrix contents:" << endl;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cout << gc.get_real_at(i, j) << " ";
        }
        cout << endl;
    }

    return 0;
}
\endcode

### Example 9: Advanced Serialization

This example illustrates advanced serialization
and deserialization of `GenericContainer`.

\code{.cc}
#include "GenericContainer.hh"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    GenericContainer gc;
    gc["item1"] = 45.67;
    gc["item2"] = "OpenAI";

    // Serialize the GenericContainer
    int sz = gc.mem_size();
    vector<uint8_t> buffer(sz);
    gc.serialize(sz, buffer.data());

    // Deserialize into a new GenericContainer
    GenericContainer gc_new;
    gc_new.de_serialize(sz, buffer.data());

    cout << "Deserialized Item1: " << gc_new["item1"].get_real() << endl;
    cout << "Deserialized Item2: " << gc_new["item2"].get_string() << endl;

    return 0;
}
\endcode

### Example 10: Data Manipulation from Files

This example demonstrates reading formatted
data from a file and displaying it using `GenericContainer`.

\code{.cc}
#include "GenericContainer.hh"
#include <iostream>
#include <fstream>

using namespace std;

int main() {
    cout << "\n\n\n"
         << "***********************\n"
         << "      Example N.10     \n"
         << "***********************\n\n";

    try {
        GenericContainer gc1, gc2, gc3, gc4, pars;

        // Reading formatted data from a file
        gc1.readFormattedData("examples/example10_data.txt");
        gc2.readFormattedData2("examples/example10_data.txt", "#", " \t", &pars);
        gc2.to_gc(gc3);
        gc4.from_gc(gc2);

        // Print contents of the GenericContainers
        cout << "GC1 --------------------------\n";
        gc1.print(cout);
        cout << "GC2 --------------------------\n";
        gc2.print(cout);
        cout << "PARS --------------------------\n";
        pars.print(cout);
        cout << "GC2 COPY ----------------------\n";
        gc3.print(cout);
        cout << "GC2 COPY2 ---------------------\n";
        gc4.print(cout);
    } catch (std::exception &exc) {
        cout << exc.what() << '\n';
    } catch (...) {
        cout << "Unknown error\n";
    }

    cout << "ALL DONE!\n\n\n\n";

    return 0;
}
\endcode

*/

/*!
  \page License

  \verbinclude "../LICENSE.md"
*/

/*!
  \page Install Installation guide

  To compile the library, you'll need:
  - A C++ compiler (e.g., GCC, Clang, or MSVC)
  - [Rake](https://ruby.github.io/rake/), a build automation tool.

  ### Compile the Library

  Open a terminal in the project's root directory and run the following command:

  \code{.bash}
  rake
  \endcode

  or if the OS is not detected correctly

  \code{bash}
  rake build_osx   # compile for OSX
  rake build_linux # compile for linux
  rake build_win   # compile for WINDOWS
  \endcode

  This will compile the source files and generate the library files in the following structure:

  \code{.text}
  lib/
    ├── include/  # Header files
    └── lib/      # Compiled library files
  \endcode

  ### check the library

  To run the tests

  \code{.bash}
  rake run       # for linux/OSX
  rake run_win   # for windows
  \endcode

  ### Integrate into Your Project

  - **Include Headers**: Add the `include/` directory to your project's include path.
    \code{.bash}
    g++ -I/path/to/lib/include your_project.cpp -o your_project
    \endcode

  - **Link the Library**: Use the `-L` and `-l` flags to link against the library:
    \code{.bash}
    g++ -L/path/to/lib/lib -lyourlibrary your_project.cpp -o your_project
    \endcode

  ### Verify the Installation

  Compile and run your project to verify that it works with the newly linked library.
*/
