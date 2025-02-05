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
 |      Universita` degli Studi di Trento                                   |
 |      email: enrico.bertolazzi@unitn.it                                   |
 |                                                                          |
\*--------------------------------------------------------------------------*/

//
// file: from_toml.cc
//

#ifdef __clang__
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-dtor"
#pragma clang diagnostic ignored "-Wshadow-field-in-constructor"
#pragma clang diagnostic ignored "-Wdeprecated-literal-operator"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif

#include "GenericContainer/GenericContainerInterface_toml.hh"

#include <iterator>
#include <sstream>
#include <fstream>
#include <string>
#include <complex>
#include <cstdint>
#include <regex>
#include <limits>
#include <cstdlib>

#include "toml++/toml.hpp"

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
  
  /*
  //
  //    __                        _                  _
  //   / _|_ __ ___  _ __ ___    | |_ ___  _ __ ___ | |
  //  | |_| '__/ _ \| '_ ` _ \   | __/ _ \| '_ ` _ \| |
  //  |  _| | | (_) | | | | | |  | || (_) | | | | | | |
  //  |_| |_|  \___/|_| |_| |_|___\__\___/|_| |_| |_|_|
  //                         |_____|
  */

  static
  bool
  TOML_to_GC( toml::node const & node, GenericContainer & gc ) {
    // Controlla il tipo di nodo
    bool ok{true};
    
    if ( node.is_string() ) {

      gc.set_string(node.as_string()->get());

    } else if ( node.is_integer() ) {

      std::int64_t tmp{ node.as_integer()->get() };
      if ( tmp >= numeric_limits<int32_t>::min() && tmp <= numeric_limits<int32_t>::max() ) {
        gc.set_int( std::int32_t(tmp) );
      } else {
        gc.set_long( tmp );
      }

    } else if ( node.is_floating_point() ) {

      gc.set_real( node.as_floating_point()->get() );

    } else if ( node.is_boolean() ) {

      gc.set_bool( node.as_boolean()->get() );

    } else if ( node.is_date() ) {

      std::ostringstream oss;
      oss << node.as_date()->get();
      gc["$date"] = oss.str();

    } else if ( node.is_time() ) {

      std::ostringstream oss;
      oss << node.as_time()->get();
      gc["$time"] = oss.str();

    } else if ( node.is_date_time() ) {

      std::ostringstream oss;
      oss << node.as_date_time()->get();
      gc["$date_time"] = oss.str();

    } else if ( node.is_table() ) {

      GC::map_type & gc_map{ gc.set_map() };
      for ( const auto& [key, value] : *node.as_table() ) {
        GenericContainer & gc1{ gc_map[std::string(key.str())] };
        ok = TOML_to_GC( value, gc1 );
        if ( !ok ) break;
      }

    } else if ( node.is_array_of_tables() ) {

      gc.set_vector();
      size_t i{0};
      for ( const auto & value : *node.as_array() ) {
        ok = TOML_to_GC( value, gc[i++] );
        if ( !ok ) break;
      }

    } else if ( node.is_array() ) {
    
      auto ary = node.as_array();

      bool is_string_array  { true };
      bool is_integer_array { true };
      bool is_float_array   { true };
      for ( const auto & value : *ary ) {
        if ( !value.is_string()         ) is_string_array  = false;
        if ( !value.is_integer()        ) is_integer_array = false;
        if ( !value.is_floating_point() ) is_float_array   = false;
      }
      size_t N{ ary->size() };
      if ( is_string_array ) {

        GC::vec_string_type & V{ gc.set_vec_string() };
        V.reserve( N );
        for ( const auto & value : *ary ) V.emplace_back( value.as_string()->get() );

      } else if (is_integer_array) {

        GC::vec_int_type & V{ gc.set_vec_int() };
        V.reserve( N );
        for ( const auto & value : *ary ) V.emplace_back( value.as_integer()->get() );

      } else if (is_float_array) {

        GC::vec_real_type & V{ gc.set_vec_real() };
        V.reserve( N );
        for ( const auto & value : *ary ) V.emplace_back( value.as_floating_point()->get() );

      } else {

        gc.set_vector();
        size_t i{0};
        for ( const auto& value : *ary ) {
          ok = TOML_to_GC( value, gc[i++] );
          if ( !ok ) break;
        }

      }

    } else {

      gc.clear();
      ok = false;

    }
    return ok;
  }

  bool
  GenericContainer::from_toml( istream_type & stream ) {
    bool ok{true};
    try {
      // deserialize the loaded file contents.
      ok = TOML_to_GC( toml::parse(stream), *this );
      if ( ok ) this->collapse();
    }
    catch ( std::exception const & e ) {
      std::cerr << "GenericContainer::from_toml: " << e.what() << '\n';
      ok = false;
    }
    catch ( ... ) {
      std::cerr << "GenericContainer::from_toml: failed\n";
      ok = false;
    }
    return ok;
  }

}

//
// eof: from_toml.cc
//
