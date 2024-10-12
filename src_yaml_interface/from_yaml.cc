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

//
// file: GenericContainerYaml.cc
//

#ifdef __clang__
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-dtor"
#endif

#include "GenericContainer/GenericContainerInterface_yaml.hh"

#include <iterator>
#include <sstream>
#include <fstream>
#include <string>
#include <complex>
#include <cstdint>
#include <regex>
#include <limits>
#include <cstdlib>

//#include "yaml-cpp/yaml.h"
#include "fkYAML/node.hpp"

namespace GC_namespace {

  using std::istreambuf_iterator;
  using std::complex;
  using std::regex;
  using std::smatch;
  using std::regex_match;
  using std::strtoull;
  using std::strtoll;
  using std::strtod;
  using std::stod;
  using std::numeric_limits;
  using std::size_t;

#if 0

  // Definizione della union per contenere diversi tipi
  union MyValue {
    bool            bool_value;
    int32_t         int32_value;
    int64_t         int64_value;
    uint32_t        uint32_value;
    uint64_t        uint64_value;
    double          double_value;
    complex<double> complex_value;
    void *          pointer_value;
    // Costruttore di default
    MyValue() {}
    // Distruttore
    ~MyValue() {}
  };

  // Enum per indicare il tipo attualmente memorizzato nella union
  enum class ValueType {
    BOOL,
    INT32,
    INT64,
    UINT32,
    UINT64,
    DOUBLE,
    COMPLEX,
    STRING,
    POINTER
  };

  // Funzione per determinare il tipo e restituire il dato nella union
  static
  ValueType
  parse_value( string const & input, MyValue & value ) {

    if ( input.empty() ) return ValueType::STRING; // stringa vuota == stringa!

    // Regex per riconoscere numeri complessi (es: "3.5+4.2i" o "-2.5-3.1i")
    static regex complex_regex(R"(^([+-]?\d+(\.\d+)?)[+-](\d+(\.\d+)?)i$)");
    static regex pointer_regex(R"(^0x(\d+)$)");

    // Rimuovi spazi bianchi
    string trimmed{input};
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);

    // Controllo per bool
    if ( trimmed == "true" || trimmed == "TRUE" || trimmed == "T" ) {
      value.bool_value = true;
      return ValueType::BOOL;
    }
    if ( trimmed == "false" || trimmed == "FALSE" || trimmed == "F"  ) {
      value.bool_value = false;
      return ValueType::BOOL;
    }

    // controllo per pointer
    char * end{nullptr};
    smatch match;
    if ( regex_match(trimmed, match, pointer_regex) ) {
      unsigned long long ptr = strtoull(trimmed.c_str()+2, &end, 16);
      if ( *end == '\0' ) {
        value.pointer_value = reinterpret_cast<void*>(ptr);
        return ValueType::POINTER;
      }
    }

    // Controllo per complex
    if ( regex_match(trimmed, match, complex_regex) ) {
      value.complex_value = complex<double>(
        stod(match[1].str()),
        stod(match[3].str())
      );
      return ValueType::COMPLEX;
    }

    if ( trimmed[0] != '-' ) { // provo unsigned
      uint64_t uint64Val = strtoull(trimmed.c_str(), &end, 10);
      if ( *end == '\0' ) {
        if ( uint64Val <= numeric_limits<uint32_t>::max() ) {
          value.uint32_value = uint32_t(uint64Val);
          return ValueType::UINT32;
        } else {
          value.uint64_value = uint64Val;
          return ValueType::UINT64;
        }
      }
    }

    // provo signed
    int64_t int64Val = strtoll(trimmed.c_str(), &end, 10);
    if (*end == '\0' ) {
      if ( int64Val >= numeric_limits<int32_t>::min() &&
           int64Val <= numeric_limits<int32_t>::max() ) {
        value.int32_value = int32_t(int64Val);
        return ValueType::INT32;
      } else {
        value.int64_value = int64Val;
        return ValueType::INT64;
      }
    }

    // Controllo per double
    double double_value = std::strtod(trimmed.c_str(), &end);
    if (*end == '\0') {
      value.double_value = double_value;
      return ValueType::DOUBLE;
    }

    // Se non ha riconosciuto nessun tipo valido
    return ValueType::STRING;
  }

#endif

  static
  bool
  YAML_to_GC( fkyaml::node const & node, GenericContainer & gc ) {
    // Controlla il tipo di nodo
    bool ok{true};
    switch (node.get_type()) {
      case fkyaml::node_type::NULL_OBJECT: {
        gc.clear();
        break;
      }
      case fkyaml::node_type::BOOLEAN: {
        gc.set_bool( node.get_value<bool>() );
        break;
      }
      case fkyaml::node_type::INTEGER: {
        std::int64_t tmp{ node.get_value<std::int64_t>() };
        if ( tmp >= numeric_limits<int32_t>::min() && tmp <= numeric_limits<int32_t>::max() ) {
          gc.set_int( std::int32_t(tmp) );
        } else {
          gc.set_long( tmp );
        }
        break;
      }
      case fkyaml::node_type::FLOAT: {
        gc = node.get_value<double>();
        break;
      }
      case fkyaml::node_type::STRING: {
        gc.set_string( node.get_value<string>() );
        break;
      }
      case fkyaml::node_type::SEQUENCE: {
        size_t N{node.size()};
        vector_type & V{ gc.set_vector( N ) };
        for ( size_t i{0}; ok && i < N; ++i )
          ok = YAML_to_GC( node[i], V[i] );
        break;
      }
      case fkyaml::node_type::MAPPING: {
        map_type & M{ gc.set_map() };
        auto & mapping = node.get_value_ref<const fkyaml::node::mapping_type&>();
        for ( auto & it : mapping ) {
          GenericContainer & gcm = M[it.first.get_value<string>()];
          ok = YAML_to_GC( it.second, gcm );
          if ( !ok ) break;
        }
        break;
      }
    }
    return ok;
  }

  bool
  GenericContainer::from_yaml( istream_type & stream ) {
    bool ok{true};
    try {
      // deserialize the loaded file contents.
      fkyaml::node root = fkyaml::node::deserialize(stream);
      ok = YAML_to_GC( root, *this );
      if ( ok ) this->collapse();
    }
    catch ( std::exception const & e ) {
      std::cerr << "GenericContainer::from_yaml: " << e.what() << '\n';
      ok = false;
    }
    catch ( ... ) {
      std::cerr << "GenericContainer::from_yaml: failed\n";
      ok = false;
    }
    return ok;
  }

}

//
// eof: from_yaml.cc
//
