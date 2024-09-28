//
//  main.cpp
//  genconjson
//
//  Created by Nicola Dal Bianco on 08/11/17.
//  Copyright © 2017 Nicola Dal Bianco. All rights reserved.
//

#pragma once

#ifndef GC_JSON_INTERFACE_HH
#define GC_JSON_INTERFACE_HH

#include "GenericContainer.hh"

namespace GC_namespace {

  using std::vector;

  extern string const GC_JSON_IM_UNIT;     //!< Key for the imaginary unit string in JSON options (default "i").
  extern string const GC_JSON_MAT_ORDER;   //!< Key for the matrix ordering option in JSON (controls how matrices are serialized).
  extern string const GC_JSON_PRETTY;      //!< Key to enable pretty (human-readable) JSON formatting (default: false).
  extern string const GC_JSON_INDENT_CHAR; //!< Key for the indentation character used in pretty printing (default: ' ').
  extern string const GC_JSON_INDENT_NUM;  //!< Key for the number of indentation characters in pretty-printed JSON (default: 4).

  //!
  //! \addtogroup JSON
  //!
  //! @{

  extern int_type const GC_JSON_column_major;  //!< Use column-major ordering when converting matrices to JSON.
  extern int_type const GC_JSON_row_major;     //!< Use row-major ordering when converting matrices to JSON.
  extern int_type const GC_JSON_none;          //!< Do not convert vectors of vectors back to matrices when decoding JSON.

  //!
  //! Convert a JSON file  to a `GenericContainer`.
  //!
  //! This function reads JSON data from the provided input stream and
  //! populates the given `GenericContainer` with the parsed data.
  //!
  //! \param[in] file_name_JSON Input file name containing JSON data.
  //! \param[out] gc The `GenericContainer` to be populated.
  //! \return true if the conversion was successful, false otherwise.
  //!
  bool
  file_JSON_to_GC(
    string const     & file_name_JSON,
    GenericContainer & gc
  );

  //!
  //! Convert a JSON file stream to a `GenericContainer`.
  //!
  //! This function reads JSON data from the provided input stream and
  //! populates the given `GenericContainer` with the parsed data.
  //!
  //! \param[in] file_JSON Input stream containing JSON data.
  //! \param[out] gc The `GenericContainer` to be populated.
  //! \return true if the conversion was successful, false otherwise.
  //!
  bool
  JSON_to_GC(
    istream_type     & file_JSON,
    GenericContainer & gc
  );

  //!
  //! Convert a YAML string to a `GenericContainer`.
  //!
  //! This function parses the given YAML string and populates the
  //! specified `GenericContainer` with the resulting data.
  //!
  //! \param[in] YAML The YAML string to parse.
  //! \param[out] gc The `GenericContainer` to be populated.
  //! \return true if the conversion was successful, false otherwise.
  //!
  bool
  JSON_to_GC(
    string const     & JSON,
    GenericContainer & gc
  );

  //!
  //! Convert a vector of JSON strings to a `GenericContainer`.
  //!
  //! This function parses each string in the given vector as YAML
  //! and populates the specified `GenericContainer` with the resulting
  //! data.
  //!
  //! \param[in] JSON Vector containing JSON strings to parse.
  //! \param[out] gc The `GenericContainer` to be populated.
  //! \return true if the conversion was successful, false otherwise.
  //!
  bool
  JSON_to_GC(
    vector<string> const & JSON,
    GenericContainer     & gc
  );

  //!
  //! Convert a `GenericContainer` to a vector of JSON strings.
  //!
  //! This function converts the contents of the provided `GenericContainer`
  //! into YAML format and stores it in the specified vector.
  //!
  //! \param[in]  gc   The `GenericContainer` to convert.
  //! \param[out] JSON Vector to store the resulting JSON strings.
  //!
  void
  GC_to_JSON(
    GenericContainer const & gc,
    vector<string>         & JSON
  );

  //!
  //! Convert a `GenericContainer` to a JSON file stream.
  //!
  //! This function converts the contents of the provided `GenericContainer`
  //! into JSON format and writes it to the specified output stream.
  //!
  //! \param[in] gc The `GenericContainer` to convert.
  //! \param[out] file_JSON Output stream to write the JSON data.
  //!
  void
  GC_to_JSON(
    GenericContainer const & gc,
    ostream_type           & file_JSONL
  );

  //!
  //! @}
  //!



  //!
  //! \addtogroup JSON
  //!
  //! @{
  //!


  /*!
   * \brief Converts a `GenericContainer` object to a JSON-formatted string.
   *
   * This function serializes the contents of a `GenericContainer` to a JSON string. Matrices within the
   * container are represented as vectors of vectors, and additional formatting options can be controlled
   * via `gc_options`.
   *
   * \param[in] gc The `GenericContainer` to be converted to a JSON string.
   * \param[in] gc_options A `GenericContainer` containing formatting options (optional, default is an empty container).
   * \return A JSON string representing the contents of the `GenericContainer`.
   *
   * \par gc_options entries:
   * The following options can be set in the `gc_options` container to control how the JSON string is formatted:
   *
   * - `gc_options[GC_JSON_IM_UNIT]`: (optional, default: "i") The string to use as the imaginary unit when representing complex numbers.
   * - `gc_options[GC_JSON_MAT_ORDER]`: (optional, default: `column_major`) The matrix ordering (column-major or row-major). Uses `GCJsonMatrixOrder`.
   * - `gc_options[GC_JSON_PRETTY]`: (optional, default: false) If set to `true`, the output JSON will be pretty-printed with indentation.
   * - `gc_options[GC_JSON_INDENT_CHAR]`: (optional, default: ' ') The character to use for indentation in pretty-printed output. Allowed values: ' ', '\\t', '\\r', '\\n'.
   * - `gc_options[GC_JSON_INDENT_NUM]`: (optional, default: 4) The number of indentation characters for each level in pretty-printed output.
   *
   * \par Example Usage:
   * \code
   * GenericContainer gc;
   * gc["name"] = "John";
   * gc["age"] = 30;
   *
   * GenericContainer options;
   * options[GC_JSON_PRETTY] = true; // Enable pretty printing
   * options[GC_JSON_INDENT_NUM] = 2; // Set indent level to 2 spaces
   *
   * std::string json_str = GC_namespace::genericContainerToJsonString(gc, options);
   * std::cout << json_str << std::endl;  // Outputs a prettified JSON string
   * \endcode
   */
  std::string
  genericContainerToJsonString(
    GenericContainer const & gc,
    GenericContainer const & gc_options = GenericContainer()
  );


  /*!
   * \brief Parses a JSON string and converts it into a `GenericContainer` object.
   *
   * This function converts a JSON-formatted string into a `GenericContainer`. It throws an exception
   * if parsing the JSON string fails. Pointers represented as strings in the JSON will remain as
   * strings in the `GenericContainer`.
   *
   * \param[in] json The input JSON string to parse.
   * \param[out] gc_output The `GenericContainer` that will store the parsed data.
   * \param[in] gc_options A `GenericContainer` containing parsing options (optional, default is an empty container).
   *
   * \par gc_options entries:
   * - `gc_options[GC_JSON_MAT_ORDER]`: (optional, default: `none`) Controls how matrices should be handled when decoding JSON.
   *   By default (`none`), the JSON vector of vectors is not automatically converted back to matrices.
   *
   * \par Example Usage:
   * \code
   * std::string json_str = R"({"name": "John", "age": 30})";
   * GenericContainer gc;
   * GC_namespace::jsonStringToGenericContainer(json_str, gc);
   *
   * std::cout << "Name: " << gc["name"].to_string() << std::endl;  // Outputs "John"
   * std::cout << "Age: " << gc["age"].to_int() << std::endl;        // Outputs 30
   * \endcode
   */
  void
  jsonStringToGenericContainer(
    std::string      const & json,
    GenericContainer       & gc_output,
    GenericContainer const & gc_options = GenericContainer()
  );

  /*!
   * \brief Writes a real number to an output stream.
   *
   * This utility function writes a floating-point number to the provided output stream in
   * a standard format.
   *
   * \param[in] number The real number to write.
   * \param[out] out The output stream where the number will be written.
   */
  void
  real_to_stream( real_type number, ostream_type & out );

  /*!
   * \brief Writes a complex number to an output stream.
   *
   * This function writes a complex number to the provided output stream in a format
   * suitable for JSON or other textual representations. The imaginary unit is specified
   * by the user via the `im_unit` parameter.
   *
   * \param[in] number The complex number to write.
   * \param[out] out The output stream where the complex number will be written.
   * \param[in] im_unit The string representing the imaginary unit (e.g., "i").
   */
  void
  complex_to_stream(
    complex_type const & number,
    ostream_type       & out,
    std::string  const & im_unit
  );

  //!
  //! @}
  //!
}

#endif

