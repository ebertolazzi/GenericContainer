/*!
  \defgroup GC GenericContainer C Interface
  \brief This module provides the C interface for the GenericContainer class.

  The GenericContainer (GC) module offers a flexible container for storing various data types such as booleans, integers, floating-point numbers, complex numbers, and strings.
  It also supports structured data types such as vectors and maps. The C interface allows for seamless interaction with the container, enabling users to perform operations
  like adding elements, retrieving data, and managing hierarchical structures.

  Functionality includes:
  - Creation and deletion of GenericContainers
  - Type setting and retrieval for various supported types
  - Operations for vectors, matrices, and maps
  - Support for complex numbers and serialization

  ## Basic Usage Pattern

  1. **Initialization**: Create an empty container.
  2. **Setting Data**: Populate the container with the desired data type.
  3. **Accessing Data**: Retrieve data stored within the container.
  4. **Manipulation**: Modify or update data within the container.

  Let's go through these steps in more detail.

  ## Initialization of a `GenericContainer`

  To start using a `GenericContainer`, you need to initialize it using the `GC_init()` function.
  Once initialized, the container is ready to store any supported data type.

  \code{.c}
  #include "GenericContainerCinterface.h"

  int main() {
      // Initialize an empty container
      GenericContainer gc;
      GC_init(&gc);

      // Further operations on the container

      GC_clear(&gc);  // Clear the container at the end
      return 0;
  }
  \endcode

  ## Setting Data in the `GenericContainer`

  The `GenericContainer` can store different types of data.
  Below are examples of how to set various types of data.

  ### Set an Integer

  To store an integer, use the `GC_set_int()` function.

  \code{.c}
  GC_set_int(&gc, 42);
  \endcode

  ### Set a Real Number

  To store a floating-point number, use `GC_set_real()`:

  \code{.c}
  GC_set_real(&gc, 3.14159);
  \endcode

  ### Set a String

  You can store a string using the `GC_set_string()` function:

  \code{.c}
  GC_set_string(&gc, "Hello, GenericContainer!");
  \endcode

  ### Set a Vector of Integers

  If you want to store an array of integers, use the `GC_set_vector_of_int()` function:

  \code{.c}
  int values[] = {1, 2, 3, 4, 5};
  GC_set_vector_of_int(&gc, values, 5);
  \endcode

  ## Accessing Data from the `GenericContainer`

  Once data is stored in the `GenericContainer`, you can retrieve it using the appropriate getter functions.

  ### Retrieve an Integer

  \code{.c}
  int value = GC_get_int(&gc);
  printf("Integer value: %d\n", value);
  \endcode

  ### Retrieve a Real Number

  \code{.c}
  double real_value = GC_get_real(&gc);
  printf("Real value: %f\n", real_value);
  \endcode

  ### Retrieve a String

  \code{.c}
  const char* str = GC_get_string(&gc);
  printf("String: %s\n", str);
  \endcode

  ### Retrieve Elements from a Vector of Integers

  To retrieve elements from a vector, use `GC_get_vector_size()` to get the size of the vector, and `GC_get_int_at_pos()` to access individual elements.

  \code{.c}
  int size = GC_get_vector_size(&gc);
  for (int i = 0; i < size; ++i) {
      int elem = GC_get_int_at_pos(&gc, i);
      printf("Element %d: %d\n", i, elem);
  }
  \endcode

  ## Advanced Manipulation: Vectors and Maps

  The `GenericContainer` also supports more complex structures like vectors of vectors or maps (key-value pairs).
  This allows you to represent hierarchical or complex data.

  ### Creating a Vector of `GenericContainer`

  You can create a vector of `GenericContainer` objects, where each element may hold a different data type:

  \code{.c}
  GC_set_vector(&gc, 3);  // Initialize a vector of 3 elements

  GC_push_vector_position(0);  // Select the first element
  GC_set_int(&gc, 10);         // Set an integer in the first element

  GC_push_vector_position(1);  // Select the second element
  GC_set_real(&gc, 2.718);     // Set a real number in the second element
  \endcode

  ### Creating a Map

  You can create a map where each key is associated with a value. For example, to map a string to another value:

  \code{.c}
  GC_set_map(&gc);                // Initialize a map
  GC_push_map_position("key");    // Insert an element with key "key"
  GC_set_string(&gc, "value");    // Set a string value for the key
  \endcode

  ## Cleanup and Deallocation

  At the end of the lifecycle of a `GenericContainer`, it's important to release the memory using `GC_clear()`:

  \code{.c}
  GC_clear(&gc);
  \endcode

  ## Error Handling

  Many `GenericContainer` functions return an error code to help you detect and handle issues. For example, after setting an integer, check the return code:

  \code{.c}
  int ret = GC_set_int(&gc, 42);
  if (ret != 0) {
      printf("Error setting integer\n");
  }
  \endcode

  ## Key Functions

  | Function                                                   | Description                                    |
  |------------------------------------------------------------|------------------------------------------------|
  | `GC_init(GenericContainer*)`                               | Initializes a `GenericContainer`.              |
  | `GC_set_int(GenericContainer*, int)`                       | Sets an integer in the container.              |
  | `GC_set_real(GenericContainer*, double)`                   | Sets a floating-point number in the container. |
  | `GC_set_string(GenericContainer*, const char*)`            | Sets a string in the container.                |
  | `GC_set_vector_of_int(GenericContainer*, const int*, int)` | Sets an integer vector.                        |
  | `GC_get_int(const GenericContainer*)`                      | Retrieves an integer from the container.       |
  | `GC_get_real(const GenericContainer*)`                     | Retrieves a floating-point number.             |
  | `GC_get_string(const GenericContainer*)`                   | Retrieves a string.                            |
  | `GC_clear(GenericContainer*)`                              | Frees the memory used by the container.        |

*/
