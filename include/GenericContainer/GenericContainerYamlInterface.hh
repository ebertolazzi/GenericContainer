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

#ifndef GENERIC_CONTAINER_YAML_INTERFACE_HH
#define GENERIC_CONTAINER_YAML_INTERFACE_HH

#ifdef __clang__
#pragma clang diagnostic ignored "-Wpoison-system-directories"
#endif

#include "GenericContainer.hh"

#include <istream>
#include <ostream>

namespace GC_namespace {

  using std::string;
  using std::vector;

  //!
  //! \addtogroup YAML
  //!
  //! @{

  //!
  //! Convert a YAML file  to a `GenericContainer`.
  //!
  //! This function reads YAML data from the provided input stream and
  //! populates the given `GenericContainer` with the parsed data.
  //!
  //! @param[in] file_name_YAML Input file name containing YAML data.
  //! @param[out] gc The `GenericContainer` to be populated.
  //! @return true if the conversion was successful, false otherwise.
  //!
  bool
  file_YAML_to_GC(
    string const     & file_name_YAML,
    GenericContainer & gc
  );

  //!
  //! Convert a YAML file stream to a `GenericContainer`.
  //!
  //! This function reads YAML data from the provided input stream and
  //! populates the given `GenericContainer` with the parsed data.
  //!
  //! @param[in] file_YAML Input stream containing YAML data.
  //! @param[out] gc The `GenericContainer` to be populated.
  //! @return true if the conversion was successful, false otherwise.
  //!
  bool
  YAML_to_GC(
    istream_type     & file_YAML,
    GenericContainer & gc
  );

  //!
  //! Convert a YAML string to a `GenericContainer`.
  //!
  //! This function parses the given YAML string and populates the
  //! specified `GenericContainer` with the resulting data.
  //!
  //! @param[in] YAML The YAML string to parse.
  //! @param[out] gc The `GenericContainer` to be populated.
  //! @return true if the conversion was successful, false otherwise.
  //!
  bool
  YAML_to_GC(
    string const     & YAML,
    GenericContainer & gc
  );

  //!
  //! Convert a vector of YAML strings to a `GenericContainer`.
  //!
  //! This function parses each string in the given vector as YAML
  //! and populates the specified `GenericContainer` with the resulting
  //! data.
  //!
  //! @param[in] YAML Vector containing YAML strings to parse.
  //! @param[out] gc The `GenericContainer` to be populated.
  //! @return true if the conversion was successful, false otherwise.
  //!
  bool
  YAML_to_GC(
    vector<string> const & YAML,
    GenericContainer     & gc
  );

  //!
  //! Convert a `GenericContainer` to a vector of YAML strings.
  //!
  //! This function converts the contents of the provided `GenericContainer`
  //! into YAML format and stores it in the specified vector.
  //!
  //! @param[in] gc The `GenericContainer` to convert.
  //! @param[out] YAML Vector to store the resulting YAML strings.
  //!
  void
  GC_to_YAML(
    GenericContainer const & gc,
    vector<string>         & YAML
  );

  //!
  //! Convert a `GenericContainer` to a YAML file stream.
  //!
  //! This function converts the contents of the provided `GenericContainer`
  //! into YAML format and writes it to the specified output stream.
  //!
  //! @param[in] gc The `GenericContainer` to convert.
  //! @param[out] file_YAML Output stream to write the YAML data.
  //!
  void
  GC_to_YAML(
    GenericContainer const & gc,
    ostream_type           & file_YAML
  );

  //!
  //! @}
  //!
}

#endif
