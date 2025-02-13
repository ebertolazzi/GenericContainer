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
// file: to_toml.cc
//

#include "GenericContainer/GenericContainer.hh"
#include <iomanip>
#include <cmath>

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


#include <fstream>
#include <memory>

#include "toml++/toml.hpp"

namespace GC_namespace {

  /*
  //   _          _                  _
  //  | |_ ___   | |_ ___  _ __ ___ | |
  //  | __/ _ \  | __/ _ \| '_ ` _ \| |
  //  | || (_) | | || (_) | | | | | | |
  //   \__\___/___\__\___/|_| |_| |_|_|
  //         |_____|
  */
  
  #define NEW_UNIQUE(T,V)      std::make_unique<T>(T{V})
  #define NEW_UNIQUE2(T,V1,V2) std::make_unique<T>(T{V1,V2})
  #define DEF_UNIQUE(T)        std::unique_ptr<T> node(std::make_unique<T>(T{}))

  static
  std::unique_ptr<toml::node>
  GC_to_TOML( GenericContainer const & gc ) {
    std::unique_ptr<toml::array> empty;
    switch ( gc.get_type() ) {
      case GC_type::NOTYPE:
        return empty;
      case GC_type::BOOL:    return NEW_UNIQUE( toml::value<bool>,    gc.get_bool() );
      case GC_type::INTEGER: return NEW_UNIQUE( toml::value<int64_t>, gc.get_int()  );
      case GC_type::LONG:    return NEW_UNIQUE( toml::value<int64_t>, gc.get_long() );
      case GC_type::REAL:    return NEW_UNIQUE( toml::value<double>,  gc.get_real() );
      case GC_type::COMPLEX: {
        real_type re, im;
        gc.get_complex_number( re, im );
        return NEW_UNIQUE2(toml::array,re,im);
      }
      case GC_type::STRING: return NEW_UNIQUE(toml::value<std::string>,gc.get_string());
      case GC_type::VEC_BOOL: {
        DEF_UNIQUE( toml::array );
        for ( bool vi : gc.get_vec_bool() ) node->as_array()->push_back( vi );
        return node;
        break;
      }
      case GC_type::VEC_INTEGER: {
        DEF_UNIQUE( toml::array );
        for ( auto vi : gc.get_vec_int() ) node->as_array()->push_back( vi );
        return node;
      }
      case GC_type::VEC_LONG: {
        DEF_UNIQUE( toml::array );
        for ( auto vi : gc.get_vec_long() ) node->as_array()->push_back( vi );
        return node;
      }
      case GC_type::VEC_REAL: {
        DEF_UNIQUE( toml::array );
        for ( auto vi : gc.get_vec_real() ) node->as_array()->push_back( vi );
        return node;
      }
      case GC_type::VEC_COMPLEX: {
        DEF_UNIQUE( toml::array );
        for ( auto vi : gc.get_vec_complex() )
          node->as_array()->push_back( toml::array{vi.real(),vi.imag()} );
        return node;
      }
      case GC_type::VEC_STRING: {
        DEF_UNIQUE( toml::array );
        for ( auto vi : gc.get_vec_string() ) node->as_array()->push_back( vi );
        return node;
      }
      case GC_type::VECTOR: {
        DEF_UNIQUE( toml::array );
        for ( auto vi : gc.get_vector() ) node->as_array()->push_back( std::move(*GC_to_TOML( vi )) );
        return node;
      }
      case GC_type::MAP: {
        DEF_UNIQUE( toml::table );
        for ( auto const & im : gc.get_map() ) {
          node->as_table()->insert( im.first, std::move(*GC_to_TOML( im.second )) );
        }
        return node;
      }
      case GC_type::MAT_INTEGER: {
        mat_int_type const & M{ gc.get_mat_int() };
        toml::array data;
        for ( auto v : M ) data.push_back( v );
        toml::table mat_int;
        mat_int.insert("$nrows",gc.num_rows());
        mat_int.insert("$ncols",gc.num_cols());
        mat_int.insert("$data",data);
        DEF_UNIQUE( toml::table );
        node->as_table()->insert( "$mat_integer", std::move(mat_int) );
        return node;
      }
      case GC_type::MAT_LONG: {
        mat_long_type const & M{ gc.get_mat_long() };
        toml::array data;
        for ( auto v : M ) data.push_back( v );
        toml::table mat_long;
        mat_long.insert("$nrows",gc.num_rows());
        mat_long.insert("$ncols",gc.num_cols());
        mat_long.insert("$data",data);
        DEF_UNIQUE( toml::table );
        node->as_table()->insert( "$mat_long", std::move(mat_long) );
        return node;
      }
      case GC_type::MAT_REAL: {
        mat_real_type const & M{ gc.get_mat_real() };
        toml::array data;
        for ( auto v : M ) data.push_back( v );
        toml::table mat_real;
        mat_real.insert("$nrows",gc.num_rows());
        mat_real.insert("$ncols",gc.num_cols());
        mat_real.insert("$data",data);
        DEF_UNIQUE( toml::table );
        node->as_table()->insert( "$mat_long", std::move(mat_real) );
        return node;
      }
      case GC_type::MAT_COMPLEX: {
        mat_complex_type const & M{ gc.get_mat_complex() };
        toml::array data_real;
        toml::array data_imag;
        for ( auto v : M ) data_real.push_back( v.real() );
        for ( auto v : M ) data_imag.push_back( v.imag() );
        toml::table mat_complex;
        mat_complex.insert("$nrows",gc.num_rows());
        mat_complex.insert("$ncols",gc.num_cols());
        mat_complex.insert("$data_real",data_real);
        mat_complex.insert("$data_imag",data_imag);
        DEF_UNIQUE( toml::table );
        node->as_table()->insert( "$mat_long", std::move(mat_complex) );
        return node;
      }
      case GC_type::POINTER: {
        DEF_UNIQUE( toml::array );
        std::ostringstream ostr;
        ostr << std::hex << gc.get_pointer<void*>();
        return NEW_UNIQUE(toml::value<std::string>,ostr.str());
      }
      case GC_type::VEC_POINTER: {
        DEF_UNIQUE( toml::array );
        for ( auto vi : gc.get_vec_pointer() ) {
          std::ostringstream ostr;
          ostr << std::hex << static_cast<void*>(vi);
          node->as_array()->push_back( ostr.str() );
        }
        return node;
      }
    }
    return empty;
  }

  bool
  GenericContainer::to_toml( ostream_type & stream ) const {
    bool ok{true};
    try {
      // deserialize the loaded file contents.
      std::unique_ptr<toml::node> root{ GC_to_TOML( *this ) };
      if ( ok ) stream << toml::toml_formatter(*root);
    }
    catch ( std::exception const & e ) {
      std::cerr << "GenericContainer::to_toml: " << e.what() << '\n';
      ok = false;
    }
    catch ( ... ) {
      std::cerr << "GenericContainer::to_toml: failed\n";
      ok = false;
    }
    return ok;
  }
}

//
// eof: to_toml.cc
//
