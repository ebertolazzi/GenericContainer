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
// file: to_yaml.cc
//

#include "GenericContainer/GenericContainer.hh"
#include <iomanip>
#include <cmath>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif

#include <fstream>

namespace GC_namespace
{

  /*
  //   _                                 _
  //  | |_ ___     _   _  __ _ _ __ ___ | |
  //  | __/ _ \   | | | |/ _` | '_ ` _ \| |
  //  | || (_) |  | |_| | (_| | | | | | | |
  //   \__\___/____\__, |\__,_|_| |_| |_|_|
  //         |_____|___/
  */

  void GenericContainer::to_yaml( ostream_type & stream, string_view prefix ) const
  {
    switch ( get_type() )
    {
      case GC_type::NOTYPE: stream << "null\n"; break;
      case GC_type::BOOL: stream << ( _b() ? "true" : "false" ) << '\n'; break;
      case GC_type::INTEGER: stream << _i() << '\n'; break;
      case GC_type::LONG: stream << _l() << '\n'; break;
      case GC_type::REAL: stream << _r() << '\n'; break;
      case GC_type::COMPLEX: stream << to_string( _c() ) << '\n'; break;
      case GC_type::STRING:
        string_escape( stream, _s() );
        stream << '\n';
        break;
      case GC_type::VEC_BOOL:
      {
        vec_bool_type const & v{ _v_b() };
        char const *          pre = "[ ";
        for ( bool vi : v )
        {
          stream << pre << ( vi ? "true" : "false" );
          pre = ", ";
        }
        stream << " ]\n";
        break;
      }
#define GC_PRINT_VECTOR       \
  char const * pre = "[ ";    \
  for ( auto const & vi : v ) \
  {                           \
    stream << pre << vi;      \
    pre = ", ";               \
  }                           \
  stream << " ]\n"
      case GC_type::VEC_INTEGER:
      {
        vec_int_type const & v{ _v_i() };
        GC_PRINT_VECTOR;
        break;
      }
      case GC_type::VEC_LONG:
      {
        vec_long_type const & v{ _v_l() };
        GC_PRINT_VECTOR;
        break;
      }
      case GC_type::VEC_REAL:
      {
        vec_real_type const & v{ _v_r() };
        GC_PRINT_VECTOR;
        break;
      }
      case GC_type::VEC_COMPLEX:
      {
        vec_complex_type const & v{ _v_c() };
        GC_PRINT_VECTOR;
        break;
      }
      case GC_type::VEC_STRING:
      {
        vec_string_type const & v{ _v_s() };
        char const *            pre = "[ ";
        for ( auto const & vi : v )
        {
          stream << pre;
          string_escape( stream, vi );
          pre = ", ";
        }
        stream << " ]\n";
        break;
      }
      case GC_type::VECTOR:
      {
        vector_type const & v{ _v() };
        string              prefix2{ prefix };
        prefix2 += "  ";
        if ( v.empty() ) { stream << "null\n"; }
        else
        {
          stream << '\n';
          for ( auto const & vi : v )
          {
            stream << prefix << "- ";
            vi.to_yaml( stream, prefix2 );
          }
        }
        break;
      }
      case GC_type::MAP:
      {
        map_type const & m{ _m() };
        if ( m.empty() ) { stream << "null\n"; }
        else
        {
          stream << '\n';
          string prefix2{ prefix };
          prefix2 += "  ";
          for ( auto const & im : m )
          {
            stream << prefix << im.first << ": ";
            im.second.to_yaml( stream, prefix2 );
          }
        }
        break;
      }
#define GC_PRINT_MATRIX                  \
  std::size_t NR{ M.num_rows() };           \
  std::size_t NC{ M.num_cols() };           \
  stream << '\n';                        \
  for ( std::size_t j{ 0 }; j < NC; ++j )   \
  {                                      \
    stream << prefix;                    \
    char const * pre = "- [ ";           \
    for ( std::size_t i{ 0 }; i < NR; ++i ) \
    {                                    \
      stream << pre << M( i, j );        \
      pre = ", ";                        \
    }                                    \
    stream << " ]\n";                    \
  }

#define GC_PRINT_MATRIXT                                 \
  std::size_t NR{ M.num_rows() };                           \
  std::size_t NC{ M.num_cols() };                           \
  stream << '\n';                                        \
  for ( std::size_t i{ 0 }; i < NR; ++i ) stream << prefix; \
  char const * pre = "- [ ";                             \
  for ( std::size_t j{ 0 }; j < NC; ++j )                   \
  {                                                      \
    {                                                    \
      stream << pre << M( i, j );                        \
      pre = ", ";                                        \
    }                                                    \
    stream << " ]\n";                                    \
  }

      case GC_type::MAT_INTEGER:
      {
        mat_int_type const & M{ _m_i() };
        GC_PRINT_MATRIX;
        break;
      }
      case GC_type::MAT_LONG:
      {
        mat_long_type const & M{ _m_l() };
        GC_PRINT_MATRIX;
        break;
      }
      case GC_type::MAT_REAL:
      {
        mat_real_type const & M{ _m_r() };
        GC_PRINT_MATRIX;
        break;
      }
      case GC_type::MAT_COMPLEX:
      {
        mat_complex_type const & M{ _m_c() };
        GC_PRINT_MATRIX;
        break;
      }
      case GC_type::POINTER:
        stream << std::hex << std::showbase << reinterpret_cast<uintptr_t>( _p() ) << '\n';
        break;
      case GC_type::VEC_POINTER:
      {
        vec_pointer_type const & V = this->get_vec_pointer( "" );
        stream << '\n';
        for ( auto const & vi : V )
        {
          stream << prefix << "- " << std::hex << std::showbase << reinterpret_cast<uintptr_t>( vi ) << '\n';
        }
        break;
      }
    }
  }
}  // namespace GC_namespace

//
// eof: to_yaml.cc
//
