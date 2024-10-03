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
// file: to_json.cc
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

  static
  string
  escape_string( string const & in ) {
    string res{""};
    for ( char c : in ) {
      if ( c == '"' ) res += '\\';
      res += c;
    }
    return res;
  }

  void
  GenericContainer::to_json(
    ostream_type      & stream,
    string_type const & prefix
  ) const {
    string_type indent{"  "};
    string_type prefix1{prefix+indent};
    switch (m_data_type) {
      case GC_type::NOTYPE:
        stream << "null";
        break;
      case GC_type::BOOL:
        stream << (m_data.b?"true":"false");
        break;
      case GC_type::INTEGER:
        stream << m_data.i;
        break;
      case GC_type::LONG:
        stream << m_data.l;
        break;
      case GC_type::REAL:
        stream << m_data.r;
        break;
      case GC_type::COMPLEX:
        stream << '"' << to_string(*m_data.c) << '"';
        break;
      case GC_type::STRING:
        stream << '"' << escape_string(*m_data.s) << '"';
        break;
      case GC_type::VEC_BOOL: {
        vec_bool_type const & v{*m_data.v_b};
        char const * pre{"[ "};
        for ( bool vi : v ) {
          stream << pre << (vi?"true":"false");
          pre = ", ";
        }
        stream << " ]";
        break;
      }
      #define GC_PRINT_VECTOR(VDATA)           \
        char const * pre = "[ ";               \
        for ( auto const & vi : v )            \
          { stream << pre << VDATA; pre = ", "; } \
        stream << " ]"

      case GC_type::VEC_INTEGER: {
        vec_int_type const & v{*m_data.v_i};
        GC_PRINT_VECTOR(vi);
        break;
      }
      case GC_type::VEC_LONG: {
        vec_long_type const & v{*m_data.v_l};
        GC_PRINT_VECTOR(vi);
        break;
      }
      case GC_type::VEC_REAL: {
        vec_real_type const & v{*m_data.v_r};
        GC_PRINT_VECTOR(vi);
        break;
      }
      case GC_type::VEC_COMPLEX: {
        vec_complex_type const & v{*m_data.v_c};
        GC_PRINT_VECTOR('"' << to_string(vi) << '"');
        break;
      }
      case GC_type::VEC_STRING: {
        vec_string_type const & v{*m_data.v_s};
        GC_PRINT_VECTOR('"' << escape_string(vi) << '"');
        break;
      }
      case GC_type::VECTOR: {
        vector_type const & v{*m_data.v};
        stream << '\n' << prefix;
        char const * pre{"[\n"};
        for ( auto const & vi : v ) {
          stream << pre << prefix1;
          vi.to_json(stream,prefix1);
          pre = ",\n";
        }
        stream << '\n' << prefix << "]";
        break;
      }
      case GC_type::MAP: {
        map_type const & m{*m_data.m};
        stream << '\n' << prefix;
        char const * pre{"{\n"};
        for ( auto const & im : m ) {
          stream << pre << prefix << indent << '"' << im.first <<  '"' << ": ";
          im.second.to_json(stream,prefix1);
          pre = ",\n";
        }
        stream << '\n' << prefix << "}";
        break;
      }
      #define GC_PRINT_MATRIX(MDATA)                 \
        unsigned NR{M.num_rows()};                   \
        unsigned NC{M.num_cols()};                   \
        stream << "[\n";                             \
        string pre0 = prefix1+"[ ";                  \
        for ( unsigned j{0}; j < NC; ++j ) {         \
          string pre{pre0};                          \
          for ( unsigned i{0}; i < NR; ++i )         \
            { stream << pre << MDATA; pre = ", "; }  \
          pre0 = " ],\n"+prefix1+"[ ";               \
        }                                            \
        stream << " ]\n" << prefix << "]"

      case GC_type::MAT_INTEGER: {
        mat_int_type const & M{*m_data.m_i};
        GC_PRINT_MATRIX(M(i,j));
        break;
      }
      case GC_type::MAT_LONG: {
        mat_long_type const & M{*m_data.m_l};
        GC_PRINT_MATRIX(M(i,j));
        break;
      }
      case GC_type::MAT_REAL: {
        mat_real_type const & M{*m_data.m_r};
        GC_PRINT_MATRIX(M(i,j));
        break;
      }
      case GC_type::MAT_COMPLEX: {
        mat_complex_type const & M{*m_data.m_c};
        GC_PRINT_MATRIX("'" << to_string(M(i,j)) << "'");
        break;
      }
      case GC_type::POINTER:
        stream << std::hex << std::showbase << reinterpret_cast<uintptr_t>(m_data.p);
        break;
      case GC_type::VEC_POINTER: {
        vec_pointer_type const & V = this->get_vec_pointer("");
        char const * pre{"[ "};
        for ( auto const & vi : V ) {
          stream << pre << std::hex << std::showbase << reinterpret_cast<uintptr_t>(vi);
          pre = ", ";
        }
        stream << " ]";
        break;
      }
    }
  }

}

//
// eof: to_json.cc
//
