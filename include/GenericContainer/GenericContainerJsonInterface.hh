/*--------------------------------------------------------------------------*\
 |                                                                          |
 |  Copyright (C) 2013                                                      |
 |                                                                          |
 |         , __                 , __                                        |
 |        /|/  \               /|/  \                                       |
 |         | __/ _   ,_         | __/ _   ,_                                |
 |         |   \|/  /  |  |   | |   \|/  /  |  |   |                        |
 |         |(__/|__/   |_/ \_/|/|(__/|__/   |_/ \_/|/                       |
 |                           /|                   /|                        |
 |                           \|                   \|                        |
 |                                                                          |
 |      Enrico Bertolazzi                                                   |
 |      Dipartimento di Ingegneria Industriale                              |
 |      Universita` degli Studi di Trento                                   |
 |      email: enrico.bertolazzi@unitn.it                                   |
 |                                                                          |
\*--------------------------------------------------------------------------*/

#pragma once

#ifndef GC_JSON_INTERFACE_HH
#define GC_JSON_INTERFACE_HH

#include "GenericContainer.hh"

namespace GC_namespace {

  using std::vector;

  //!
  //! \addtogroup JSON
  //!
  //! @{

  //!
  //! Convert a JSON file  to a `GenericContainer`.
  //!
  //! This function reads JSON data from the provided input stream and
  //! populates the given `GenericContainer` with the parsed data.
  //!
  //! \param[in]  file_name_JSON Input file name containing JSON data.
  //! \param[out] gc             The `GenericContainer` to be populated.
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
  //! \param[in]  file_JSON Input stream containing JSON data.
  //! \param[out] gc        The `GenericContainer` to be populated.
  //! \return true if the conversion was successful, false otherwise.
  //!
  bool
  JSON_to_GC(
    istream_type     & file_JSON,
    GenericContainer & gc
  );

  //!
  //! Convert a JSON string to a `GenericContainer`.
  //!
  //! This function parses the given JSON string and populates the
  //! specified `GenericContainer` with the resulting data.
  //!
  //! \param[in]  JSON The JSON string to parse.
  //! \param[out] gc   The `GenericContainer` to be populated.
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
  //! \param[in]  JSON Vector containing JSON strings to parse.
  //! \param[out] gc   The `GenericContainer` to be populated.
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
    ostream_type           & file_JSON
  );

  //!
  //! @}
  //!

}

#endif

