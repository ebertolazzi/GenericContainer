/*!

\page Usage

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
