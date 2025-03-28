/*!

 \defgroup JSON GenericContainer-JSON Interface

 \brief This module provides interface with JSON files.

 The GenericContainer (GC) module offers a flexible container for storing various data types
 such as booleans, integers, floating-point numbers, complex numbers, and strings.
 It also supports structured data types such as vectors and maps. The C++ interface
 allows for seamless interaction with the container, enabling users to perform operations
 like adding elements, retrieving data, and managing hierarchical structures.

 ## Overview

 This tutorial explains how to use the `GenericContainer` JSON interface for
 converting between JSON data and the `GenericContainer` object.

 The `GenericContainer` is a flexible structure that can store various data types
 like booleans, integers, floating-point numbers, complex numbers, strings, vectors,
 and maps. The interface provides multiple ways to convert JSON files, streams, or strings
 into `GenericContainer` objects and vice versa.

 ## Loading JSON Data into a GenericContainer

 There are several ways to load JSON data into a `GenericContainer`.

 ### Loading JSON from a File

 The `file_JSON_to_GC` function reads JSON data from a file and populates
 a `GenericContainer`. This is useful for handling configuration files or other
 data stored in JSON format.

 \code
 bool file_JSON_to_GC( std::string const & file_name_JSON, GenericContainer & gc );
 \endcode

 Example:

 \code
 GenericContainer gc;
 std::string file_name = "config.JSON";

 if (file_JSON_to_GC(file_name, gc)) {
   std::cout << "JSON file successfully loaded into GenericContainer." << std::endl;
 } else {
   std::cerr << "Failed to load JSON file." << std::endl;
 }
 \endcode

 ### Loading JSON from an Input Stream

 The `JSON_to_GC` function allows loading JSON data from an input stream,
 such as a file stream or a string stream.

 \code
 bool JSON_to_GC( istream_type & file_JSON, GenericContainer & gc );
 \endcode

 Example:
 \code
 GenericContainer gc;
 std::ifstream file_stream("config.JSON");

 if (JSON_to_GC(file_stream, gc)) {
   std::cout << "JSON stream successfully loaded into GenericContainer." << std::endl;
 } else {
   std::cerr << "Failed to load JSON stream." << std::endl;
 }
 \endcode

 ### Loading JSON from a String

 You can convert a JSON string directly into a `GenericContainer` using the `JSON_to_GC` function.

 \code
 bool JSON_to_GC( std::string const & JSON, GenericContainer & gc );
 \endcode

 Example:
 \code
 GenericContainer gc;
 std::string JSON_data = R"(
 {
   "products": [
     {
       "id": 1,
       "name": "Laptop",
       "price": 999.99,
       "in_stock": true
     },
     {
       "id": 2,
       "name": "Smartphone",
       "price": 499.99,
       "in_stock": false
     }
   ],
   "meta": {
     "total_products": 2,
     "available_products": 1
   }
 }
 )";

 if (JSON_to_GC(JSON_data, gc)) {
   std::cout << "JSON string successfully loaded into GenericContainer." << std::endl;
 } else {
   std::cerr << "Failed to load JSON string." << std::endl;
 }
 \endcode

 ### Loading JSON from a Vector of Strings

 If you have multiple JSON strings in a vector,
 you can load them into a `GenericContainer` using the `JSON_to_GC` function.

 \code
 bool JSON_to_GC( std::vector<std::string> const & JSON, GenericContainer & gc );
 \endcode

 Example:
 \code
 GenericContainer gc;
 std::vector<std::string> JSON_fragments = {
   "{",
   "  \"products\": [",
   "    {",
   "      \"id\": 1,",
   "      \"name\": "Laptop",",
   "      \"price\": 999.99,",
   "      \"in_stock\": true",
   "    },",
   "    {",
   "      \"id\": 2,",
   "      \"name\": "Smartphone",",
   "      \"price\": 499.99,",
   "      \"in_stock\": false",
   "    }",
   "  ],",
   "  \"meta\": {",
   "    "total_products": 2,",
   "    "available_products": 1",
   "  }",
   "}"
 };

 if (JSON_to_GC(JSON_fragments, gc)) {
   std::cout << "JSON fragments successfully loaded into GenericContainer." << std::endl;
 } else {
   std::cerr << "Failed to load JSON fragments." << std::endl;
 }
 \endcode

 ## Converting a GenericContainer to JSON

 You can also convert a `GenericContainer` back to JSON format.

 ### Converting to a Vector of JSON Strings

 The `GC_to_JSON` function converts the contents of
 a `GenericContainer` into a vector of JSON strings.

 \code
 void GC_to_JSON( GenericContainer const & gc, std::vector<std::string> & JSON );
 \endcode

 Example:
 \code
 GenericContainer gc;
 gc["key1"] = "value1";
 gc["key2"] = 42;

 std::vector<std::string> JSON_output;
 GC_to_JSON(gc, JSON_output);

 for (const auto &line : JSON_output) {
     std::cout << line << std::endl;
 }
 \endcode

 ### Converting to a JSON File Stream

 You can write the contents of a `GenericContainer` to an
 output stream (e.g., a file) in JSON format using the `GC_to_JSON` function.

 \code
 void GC_to_JSON( GenericContainer const & gc, ostream_type & file_JSON );
 \endcode

 Example:
 \code
 GenericContainer gc;
 gc["key1"] = "value1";
 gc["key2"] = 42;

 std::ofstream file_stream("output.JSON");
 GC_to_JSON(gc, file_stream);

 std::cout << "JSON data written to output.JSON" << std::endl;
 \endcode

*/
