/*--------------------------------------------------------------------------*\
 |  Range-checked narrowing tests. [newbehavior]: the current code performs |
 |  unchecked static_casts (get_value<int> from LONG truncates silently;    |
 |  huge doubles pass the isInteger() test and the cast is UB).             |
 |  Enabled by Phase 3's checked_narrow<> migration.                        |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <limits>
#include <stdexcept>

using namespace GC_namespace;

TEST_CASE( "boundary values inside the target range convert", "[convert]" )
{
  GenericContainer gc;

  gc.set_long( std::numeric_limits<int_type>::max() );
  CHECK( gc.get_as_int() == std::numeric_limits<int_type>::max() );

  gc.set_long( std::numeric_limits<int_type>::min() );
  CHECK( gc.get_as_int() == std::numeric_limits<int_type>::min() );

  gc.set_real( 2147483647.0 );  // INT32_MAX exactly representable
  CHECK( gc.get_as_int() == 2147483647 );

  gc.set_real( -2147483648.0 );
  CHECK( gc.get_as_int() == -2147483648 );
}

TEST_CASE( "LONG beyond int32 range is rejected by get_as_int", "[convert][newbehavior]" )
{
  GenericContainer gc;
  gc.set_long( 1LL << 40 );
  CHECK_THROWS_AS( gc.get_as_int(), std::runtime_error );

  gc.set_long( -( 1LL << 40 ) );
  CHECK_THROWS_AS( gc.get_as_int(), std::runtime_error );

  int_type out{ 0 };
  gc.set_long( 1LL << 40 );
  CHECK_THROWS_AS( gc.get_value( out ), std::runtime_error );
}

TEST_CASE( "huge integral-valued doubles are rejected, not UB-cast", "[convert][newbehavior]" )
{
  GenericContainer gc;
  gc.set_real( 1e300 );
  CHECK_THROWS_AS( gc.get_as_int(), std::runtime_error );
  CHECK_THROWS_AS( gc.get_as_long(), std::runtime_error );

  gc.set_real( 2147483648.0 );  // INT32_MAX + 1
  CHECK_THROWS_AS( gc.get_as_int(), std::runtime_error );

  vec_int_type vi;
  gc = vec_real_type{ 1e300 };
  CHECK_THROWS_AS( gc.copyto_vec_int( vi ), std::runtime_error );

  vec_long_type vl;
  gc = vec_real_type{ 1e300 };
  CHECK_THROWS_AS( gc.copyto_vec_long( vl ), std::runtime_error );
}

TEST_CASE( "NaN and infinity are rejected for integral conversion", "[convert][newbehavior]" )
{
  GenericContainer gc;
  gc.set_real( std::numeric_limits<real_type>::quiet_NaN() );
  CHECK_THROWS_AS( gc.get_as_int(), std::runtime_error );

  gc.set_real( std::numeric_limits<real_type>::infinity() );
  CHECK_THROWS_AS( gc.get_as_long(), std::runtime_error );
}

TEST_CASE( "negative values are rejected for unsigned conversion", "[convert]" )
{
  GenericContainer gc;
  gc.set_int( -1 );
  CHECK_THROWS_AS( gc.get_as_uint(), std::runtime_error );
  CHECK_THROWS_AS( gc.get_as_ulong(), std::runtime_error );
}

TEST_CASE( "complex never converts through get_number", "[convert]" )
{
  // Characterization: get_number rejects COMPLEX even with zero imaginary
  // part; get_value<int> instead accepts zero-imaginary integral complex.
  GenericContainer gc;
  gc.set_complex( 1.0, 0.5 );
  CHECK_THROWS_AS( gc.get_number(), std::runtime_error );

  int_type out{ 0 };
  CHECK_THROWS_AS( gc.get_value( out ), std::runtime_error );

  gc.set_complex( 4.0, 0.0 );
  CHECK_THROWS_AS( gc.get_number(), std::runtime_error );
  gc.get_value( out );
  CHECK( out == 4 );
  CHECK( gc.get_complex_number() == complex_type( 4.0, 0.0 ) );
}

TEST_CASE( "fractional real cannot convert to integral", "[convert]" )
{
  GenericContainer gc;
  gc.set_real( 1.5 );
  int_type i{ 0 };
  CHECK_THROWS_AS( gc.get_value( i ), std::runtime_error );
  long_type l{ 0 };
  CHECK_THROWS_AS( gc.get_value( l ), std::runtime_error );
}
