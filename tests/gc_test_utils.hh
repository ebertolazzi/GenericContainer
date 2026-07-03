/*--------------------------------------------------------------------------*\
 |  Shared helpers for the GenericContainer Catch2 test battery.            |
\*--------------------------------------------------------------------------*/

#pragma once

#include "GenericContainer/GenericContainer.hh"

#include <random>
#include <string>

namespace gc_test
{

  using GC_namespace::GenericContainer;

  //! Structural deep equality, checked through BOTH mechanisms the library
  //! offers: the C++20 operator== (variant/Box deep equality) and the
  //! diagnostic compare_content (empty diff = equal). They must agree.
  inline bool gc_equal( GenericContainer const & a, GenericContainer const & b )
  {
    bool const eq = ( a == b );
    bool const cc = a.compare_content( b ).empty() && b.compare_content( a ).empty();
    return eq && cc;
  }

  //! Deterministic random container generator for property-style round trips.
  //! POINTER/VEC_POINTER are excluded: pointer values are process-specific and
  //! meaningless through serialization.
  inline GenericContainer make_random_gc( std::mt19937 & rng, int const depth = 0 )
  {
    using GC_namespace::complex_type;
    using GC_namespace::int_type;
    using GC_namespace::long_type;
    using GC_namespace::real_type;

    std::uniform_int_distribution<int>       type_pick( 0, depth >= 3 ? 10 : 12 );
    std::uniform_int_distribution<int>       size_pick( 0, 6 );
    std::uniform_int_distribution<int_type>  int_pick( -1000, 1000 );
    std::uniform_int_distribution<long_type> long_pick( -5'000'000'000LL, 5'000'000'000LL );
    std::uniform_real_distribution<double>   real_pick( -1e6, 1e6 );

    GenericContainer gc;
    switch ( type_pick( rng ) )
    {
      case 0: break;  // NOTYPE
      case 1: gc = ( int_pick( rng ) & 1 ) == 1; break;
      case 2: gc = int_pick( rng ); break;
      case 3: gc = long_pick( rng ); break;
      case 4: gc = real_pick( rng ); break;
      case 5: gc = complex_type( real_pick( rng ), real_pick( rng ) ); break;
      case 6: gc = "str_" + std::to_string( int_pick( rng ) ); break;
      case 7:
      {
        auto & v = gc.set_vec_int( unsigned( size_pick( rng ) ) );
        for ( auto & e : v ) e = int_pick( rng );
        break;
      }
      case 8:
      {
        auto & v = gc.set_vec_real( unsigned( size_pick( rng ) ) );
        for ( auto & e : v ) e = real_pick( rng );
        break;
      }
      case 9:
      {
        auto & v = gc.set_vec_string( unsigned( size_pick( rng ) ) );
        for ( auto & e : v ) e = "s" + std::to_string( int_pick( rng ) );
        break;
      }
      case 10:
      {
        auto & m = gc.set_mat_real( unsigned( 1 + size_pick( rng ) ), unsigned( 1 + size_pick( rng ) ) );
        for ( unsigned j{ 0 }; j < m.num_cols(); ++j )
          for ( unsigned i{ 0 }; i < m.num_rows(); ++i ) m( i, j ) = real_pick( rng );
        break;
      }
      case 11:
      {
        auto & v = gc.set_vector( unsigned( size_pick( rng ) ) );
        for ( auto & e : v ) e = make_random_gc( rng, depth + 1 );
        break;
      }
      case 12:
      {
        gc.set_map();
        int const n = size_pick( rng );
        for ( int k = 0; k < n; ++k ) gc["key_" + std::to_string( k )] = make_random_gc( rng, depth + 1 );
        break;
      }
    }
    return gc;
  }

  //! A container exercising every serializable type, with fixed values.
  //! Used by print/dump smoke tests and the serialization golden pin.
  inline GenericContainer make_maximal_gc()
  {
    using GC_namespace::complex_type;
    using GC_namespace::int_type;
    using GC_namespace::long_type;

    GenericContainer gc;
    gc.set_map();
    gc["bool"]    = true;
    gc["int"]     = int_type( 42 );
    gc["long"]    = long_type( 1LL << 40 );
    gc["real"]    = 3.5;
    gc["complex"] = complex_type( 1.0, -2.0 );
    gc["string"]  = "hello";

    gc["vec_bool"].set_vec_bool( GC_namespace::vec_bool_type{ true, false, true } );
    gc["vec_int"].set_vec_int( GC_namespace::vec_int_type{ 1, -2, 3 } );
    gc["vec_long"].set_vec_long( GC_namespace::vec_long_type{ 1LL << 33, -( 1LL << 34 ) } );
    gc["vec_real"].set_vec_real( GC_namespace::vec_real_type{ 0.5, 1.5, -2.5 } );
    gc["vec_complex"].set_vec_complex(
      GC_namespace::vec_complex_type{ complex_type( 1, 2 ), complex_type( -3, 4 ) } );
    gc["vec_string"].set_vec_string( GC_namespace::vec_string_type{ "a", "bb", "ccc" } );

    auto & mi = gc["mat_int"].set_mat_int( 2, 3 );
    for ( unsigned j{ 0 }; j < 3; ++j )
      for ( unsigned i{ 0 }; i < 2; ++i ) mi( i, j ) = int_type( 10 * i + j );
    auto & ml = gc["mat_long"].set_mat_long( 2, 2 );
    for ( unsigned j{ 0 }; j < 2; ++j )
      for ( unsigned i{ 0 }; i < 2; ++i ) ml( i, j ) = long_type( ( 1LL << 35 ) + 10 * i + j );
    auto & mr = gc["mat_real"].set_mat_real( 2, 2 );
    for ( unsigned j{ 0 }; j < 2; ++j )
      for ( unsigned i{ 0 }; i < 2; ++i ) mr( i, j ) = 0.25 * ( 1 + int( i ) + 2 * int( j ) );
    auto & mc = gc["mat_complex"].set_mat_complex( 1, 2 );
    mc( 0, 0 ) = complex_type( 1, -1 );
    mc( 0, 1 ) = complex_type( -2, 2 );

    auto & v = gc["vector"].set_vector( 3 );
    v[0]      = int_type( 7 );
    v[1]      = "nested";
    v[2]      = false;

    gc["map"]["inner"] = 1.25;
    gc["map"]["deep"]["leaf"].set_vec_int( GC_namespace::vec_int_type{ 9, 8 } );
    return gc;
  }

}  // namespace gc_test
