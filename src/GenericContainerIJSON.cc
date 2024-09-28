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
// file: GenericContainerJSON.cc
//

#include "GenericContainer/GenericContainer.hh"
#include <iomanip>
#include <cmath>
#include <ctgmath>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif

#include <fstream>

namespace GC_namespace {

  /*
  //   _              _
  //  | |_ ___       (_)___  ___  _ __
  //  | __/ _ \      | / __|/ _ \| '_ \
  //  | || (_) |     | \__ \ (_) | | | |
  //   \__\___/____ _/ |___/\___/|_| |_|
  //         |_____|__/
  */

  void
  GenericContainer::to_json(
    ostream_type      & stream,
    string_type const & prefix
  ) const {
    switch (m_data_type) {
    case GC_type::NOTYPE:
      stream << prefix << "~\n";
      break;
    case GC_type::BOOL:
      stream << prefix << (this->get_bool()?"true":"false") << '\n';
      break;
    case GC_type::INTEGER:
      stream << prefix << this->get_int() << '\n';
      break;
    case GC_type::LONG:
      stream << prefix << this->get_long() << '\n';
      break;
    case GC_type::REAL:
      stream << prefix << this->get_real() << '\n';
      break;
    case GC_type::COMPLEX:
      stream << prefix << get_complex() << '\n';
      break;
    case GC_type::STRING:
      stream << prefix << "'" << this->get_string().c_str() << "'\n";
      break;
    case GC_type::VEC_BOOL: {
      vec_bool_type const & v = this->get_vec_bool();
      stream << prefix;
      char const * pre = "[ ";
      for ( bool vi : v ) {
        stream << pre << (vi?"true":"false");
        pre = ", ";
      }
      stream << " ]\n";
      break;
    }
    case GC_type::VEC_INTEGER: {
      vec_int_type const & v = this->get_vec_int();
      stream << prefix;
      char const * pre = "[ ";
      for ( int_type const & vi : v ) {
        stream << pre << vi;
        pre = ", ";
      }
      stream << " ]\n";
      break;
    }
    case GC_type::VEC_LONG: {
      vec_long_type const & v = this->get_vec_long();
      stream << prefix;
      char const * pre = "[ ";
      for ( long_type const & vi : v ) {
        stream << pre << vi;
        pre = ", ";
      }
      stream << " ]\n";
      break;
    }
    case GC_type::VEC_REAL: {
      vec_real_type const & v = this->get_vec_real();
      stream << prefix;
      char const * pre = "[ ";
      for ( double const & vi : v ) {
        stream << pre << vi;
        pre = ", ";
      }
      stream << " ]\n";
      break;
    }
    case GC_type::VEC_COMPLEX: {
      vec_complex_type const & v = this->get_vec_complex();
      stream << prefix;
      char const * pre = "[ ";
      for ( complex_type const & vi : v ) {
        stream << pre << vi;
        pre = ", ";
      }
      stream << " ]\n";
      break;
    }
    case GC_type::VEC_STRING: {
      vec_string_type const & v = this->get_vec_string();
      stream << prefix;
      char const * pre = "[ ";
      for ( auto const & vi : v ) {
        stream << pre << "'" << vi << "'";
        pre = ", ";
      }
      stream << " ]\n";
      break;
    }
    case GC_type::VECTOR: {
      vector_type const & v = this->get_vector();
      stream << '\n';
      for ( auto const & vi : v ) {
        stream << prefix << "- ";
        vi.to_json(stream,prefix+"  ");
      }
      break;
    }
    case GC_type::MAP: {
      map_type const & m = this->get_map();
      stream << '\n';
      for ( auto const & im : m ) {
        stream << prefix << '"' << im.first << "\":\n";
        im.second.to_json(stream,prefix+"  ");
      }
      break;
    }
    case GC_type::MAT_INTEGER: {
      mat_int_type const & M = this->get_mat_int("");
      stream << prefix << "[\n";
      for ( unsigned j{0}; j < M.num_cols(); ++j ) {
        stream << prefix;
        char const * pre = "[ ";
        for ( unsigned i{0}; i < M.num_rows(); ++i ) {
          stream << pre << M(i,j);
          pre = ", ";
        }
        stream << " ]\n";
      }
      stream << prefix << "]\n";
      break;
    }
    case GC_type::MAT_LONG: {
      mat_long_type const & M = this->get_mat_long("");
      stream << prefix << "[\n";
      for ( unsigned j{0}; j < M.num_cols(); ++j ) {
        stream << prefix;
        char const * pre = "[ ";
        for ( unsigned i{0}; i < M.num_rows(); ++i ) {
          stream << pre << M(i,j);
          pre = ", ";
        }
        stream << " ]\n";
      }
      stream << prefix << "]\n";
      break;
    }
    case GC_type::MAT_REAL: {
      mat_real_type const & M = this->get_mat_real("");
      stream << prefix << "[\n";
      for ( unsigned j{0}; j < M.num_cols(); ++j ) {
        stream << prefix;
        char const * pre = "[ ";
        for ( unsigned i{0}; i < M.num_rows(); ++i ) {
          stream << pre << M(i,j);
          pre = ", ";
        }
        stream << " ]\n";
      }
      stream << prefix << "]\n";
    }
    case GC_type::MAT_COMPLEX: {
      mat_complex_type const & M = this->get_mat_complex("");
      stream << prefix << "[\n";
      for ( unsigned j{0}; j < M.num_cols(); ++j ) {
        stream << prefix;
        char const * pre = "[ ";
        for ( unsigned i{0}; i < M.num_rows(); ++i ) {
          stream << pre << M(i,j);
          pre = ", ";
        }
        stream << " ]\n";
      }
      stream << prefix << "]\n";
      break;
    }
    case GC_type::POINTER:
    case GC_type::VEC_POINTER:
      { /* DA FARE */ }
      break;
    }
  }

}

//
// eof: GenericContainerJSON.cc
//
