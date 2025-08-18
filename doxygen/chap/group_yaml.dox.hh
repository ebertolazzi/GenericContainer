/*!

 \defgroup YAML GenericContainer-YAML Interface

 \brief This module provides interface with YAML files.

 The GenericContainer (GC) module offers a flexible container for storing various data types
 such as booleans, integers, floating-point numbers, complex numbers, and strings.
 It also supports structured data types such as vectors and maps. The C++ interface
 allows for seamless interaction with the container, enabling users to perform operations
 like adding elements, retrieving data, and managing hierarchical structures.

 ## Overview

 This tutorial explains how to use the `GenericContainer` YAML interface for
 converting between YAML data and the `GenericContainer` object.

 The `GenericContainer` is a flexible structure that can store various data types
 like booleans, integers, floating-point numbers, complex numbers, strings, vectors,
 and maps. The interface provides multiple ways to convert YAML files, streams, or strings
 into `GenericContainer` objects and vice versa.

 ## Loading YAML Data into a GenericContainer

 There are several ways to load YAML data into a `GenericContainer`.

 ### Loading YAML from a File

 The `file_YAML_to_GC` function reads YAML data from a file and populates
 a `GenericContainer`. This is useful for handling configuration files or other
 data stored in YAML format.

 \code
 bool file_YAML_to_GC( std::string const & file_name_YAML, GenericContainer & gc );
 \endcode

 Example:

 \code
 GenericContainer gc;
 std::string file_name = "config.yaml";

 if (file_YAML_to_GC(file_name, gc)) {
   std::cout << "YAML file successfully loaded into GenericContainer." << std::endl;
 } else {
   std::cerr << "Failed to load YAML file." << std::endl;
 }
 \endcode

 ### Loading YAML from an Input Stream

 The `YAML_to_GC` function allows loading YAML data from an input stream,
 such as a file stream or a string stream.

 \code
 bool YAML_to_GC( istream_type & file_YAML, GenericContainer & gc );
 \endcode

 Example:
 \code
 GenericContainer gc;
 std::ifstream file_stream("config.yaml");

 if (YAML_to_GC(file_stream, gc)) {
   std::cout << "YAML stream successfully loaded into GenericContainer." << std::endl;
 } else {
   std::cerr << "Failed to load YAML stream." << std::endl;
 }
 \endcode

 ### Loading YAML from a String

 You can convert a YAML string directly into a `GenericContainer` using the `YAML_to_GC` function.

 \code
 bool YAML_to_GC( std::string const & YAML, GenericContainer & gc );
 \endcode

 Example:
 \code
 GenericContainer gc;
 std::string yaml_data = R"(
     key1: value1
     key2: 42
     key3:
       - 1
       - 2
       - 3
 )";

 if (YAML_to_GC(yaml_data, gc)) {
   std::cout << "YAML string successfully loaded into GenericContainer." << std::endl;
 } else {
   std::cerr << "Failed to load YAML string." << std::endl;
 }
 \endcode

 ### Loading YAML from a Vector of Strings

 If you have multiple YAML strings in a vector,
 you can load them into a `GenericContainer` using the `YAML_to_GC` function.

 \code
 bool YAML_to_GC( std::vector<std::string> const & YAML, GenericContainer & gc );
 \endcode

 Example:
 \code
 GenericContainer gc;
 std::vector<std::string> yaml_fragments = {
     "key1: value1",
     "key2: 42",
     "key3:\n  - 1\n  - 2\n  - 3"
 };

 if (YAML_to_GC(yaml_fragments, gc)) {
   std::cout << "YAML fragments successfully loaded into GenericContainer." << std::endl;
 } else {
   std::cerr << "Failed to load YAML fragments." << std::endl;
 }
 \endcode

 ## Converting a GenericContainer to YAML

 You can also convert a `GenericContainer` back to YAML format.

 ### Converting to a YAML File Stream

 You can write the contents of a `GenericContainer` to an
 output stream (e.g., a file) in YAML format using the `GC_to_YAML` function.

 \code
 void GC_to_YAML( GenericContainer const & gc, ostream_type & file_YAML );
 \endcode

 Example:
 \code
 GenericContainer gc;
 gc["key1"] = "value1";
 gc["key2"] = 42;

 std::ofstream file_stream("output.yaml");
 GC_to_YAML(gc, file_stream);

 std::cout << "YAML data written to output.yaml" << std::endl;
 \endcode

*/
