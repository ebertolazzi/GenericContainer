/*--------------------------------------------------------------------------*\
 |                                                                          |
 |  Copyright (C) 2025                                                      |
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
 |      Università degli Studi di Trento                                    |
 |      email: enrico.bertolazzi@unitn.it                                   |
 |                                                                          |
\*--------------------------------------------------------------------------*/

//
// file: from_yaml.cc
//

#ifdef __clang__
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-dtor"
#pragma clang diagnostic ignored "-Wshadow-field-in-constructor"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
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

// #include "yaml-cpp/yaml.h"
#include "fkYAML/node.hpp"

namespace GC_namespace
{

  using std::complex;
  using std::istreambuf_iterator;
  using std::numeric_limits;
  using std::regex;
  using std::regex_match;
  using std::size_t;
  using std::smatch;
  using std::stod;
  using std::strtod;
  using std::strtoll;
  using std::strtoull;

  static bool YAML_to_GC( fkyaml::node const & node, GenericContainer & gc )
  {
    // Controlla il tipo di nodo
    bool ok{ true };
    switch ( node.get_type() )
    {
      case fkyaml::node_type::NULL_OBJECT:
      {
        gc.clear();
        break;
      }
      case fkyaml::node_type::BOOLEAN:
      {
        gc.set_bool( node.get_value<bool>() );
        break;
      }
      case fkyaml::node_type::INTEGER:
      {
        std::int64_t tmp{ node.get_value<std::int64_t>() };
        if ( tmp >= numeric_limits<int32_t>::min() && tmp <= numeric_limits<int32_t>::max() )
        {
          gc.set_int( std::int32_t( tmp ) );
        }
        else
        {
          gc.set_long( tmp );
        }
        break;
      }
      case fkyaml::node_type::FLOAT:
      {
        gc = node.get_value<double>();
        break;
      }
      case fkyaml::node_type::STRING:
      {
        gc.set_string( node.get_value<string>() );
        break;
      }
      case fkyaml::node_type::SEQUENCE:
      {
        size_t        N{ node.size() };
        vector_type & V{ gc.set_vector( static_cast<unsigned>( N ) ) };
        for ( size_t i{ 0 }; ok && i < N; ++i ) ok = YAML_to_GC( node[i], V[i] );
        break;
      }
      case fkyaml::node_type::MAPPING:
      {
        map_type & M{ gc.set_map() };
        auto &     mapping = node.get_value_ref<const fkyaml::node::mapping_type &>();
        for ( auto & it : mapping )
        {
          GenericContainer & gcm = M[it.first.get_value<string>()];
          ok                     = YAML_to_GC( it.second, gcm );
          if ( !ok ) break;
        }
        break;
      }
    }
    return ok;
  }

  bool GenericContainer::from_yaml( istream_type & stream )
  {
    bool ok{ true };
    try
    {
      // deserialize the loaded file contents.
      fkyaml::node root = fkyaml::node::deserialize( stream );
      ok                = YAML_to_GC( root, *this );
      if ( ok ) this->collapse();
    }
    catch ( std::exception const & e )
    {
      std::cerr << "GenericContainer::from_yaml: " << e.what() << '\n';
      ok = false;
    }
    catch ( ... )
    {
      std::cerr << "GenericContainer::from_yaml: failed\n";
      ok = false;
    }
    return ok;
  }

}  // namespace GC_namespace

//
// eof: from_yaml.cc
//
