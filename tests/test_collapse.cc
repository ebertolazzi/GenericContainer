/*--------------------------------------------------------------------------*\
 |  Characterization tests: collapse() of VECTOR-of-uniform content.        |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>

#include <utility>

using namespace GC_namespace;

TEST_CASE( "vector of uniform scalars collapses to typed vector", "[collapse]" )
{
  GenericContainer gc;
  auto &           v = gc.set_vector( 3 );
  v[0] = 1;
  v[1] = 2;
  v[2] = 3;
  gc.collapse();
  CHECK( gc.get_type() == GC_type::VEC_INTEGER );
  CHECK( std::as_const( gc ).get_vec_int() == vec_int_type{ 1, 2, 3 } );
}

TEST_CASE( "vector of mixed int/real collapses to vec_real", "[collapse]" )
{
  GenericContainer gc;
  auto &           v = gc.set_vector( 3 );
  v[0] = 1;
  v[1] = 2.5;
  v[2] = 3;
  gc.collapse();
  CHECK( gc.get_type() == GC_type::VEC_REAL );
  CHECK( std::as_const( gc ).get_vec_real() == vec_real_type{ 1.0, 2.5, 3.0 } );
}

TEST_CASE( "vector of bools collapses to vec_bool", "[collapse]" )
{
  GenericContainer gc;
  auto &           v = gc.set_vector( 2 );
  v[0] = true;
  v[1] = false;
  gc.collapse();
  CHECK( gc.get_type() == GC_type::VEC_BOOL );
  CHECK( std::as_const( gc ).get_vec_bool() == vec_bool_type{ true, false } );
}

TEST_CASE( "vector of equal-length numeric vectors collapses to matrix", "[collapse]" )
{
  GenericContainer gc;
  auto &           v = gc.set_vector( 2 );
  v[0] = vec_real_type{ 1.0, 2.0, 3.0 };
  v[1] = vec_real_type{ 4.0, 5.0, 6.0 };
  gc.collapse();
  CHECK( gc.get_type() == GC_type::MAT_REAL );
  CHECK( gc.get_num_elements() == 6 );
  // pin the orientation: each source vector becomes a column
  auto const & m = std::as_const( gc ).get_mat_real();
  REQUIRE( gc.num_rows() == 3 );
  REQUIRE( gc.num_cols() == 2 );
  CHECK( m( 0, 0 ) == 1.0 );
  CHECK( m( 1, 0 ) == 2.0 );
  CHECK( m( 2, 0 ) == 3.0 );
  CHECK( m( 0, 1 ) == 4.0 );
  CHECK( m( 1, 1 ) == 5.0 );
  CHECK( m( 2, 1 ) == 6.0 );
}

TEST_CASE( "heterogeneous vector stays VECTOR", "[collapse]" )
{
  GenericContainer gc;
  auto &           v = gc.set_vector( 2 );
  v[0] = 1;
  v[1] = "str";
  gc.collapse();
  CHECK( gc.get_type() == GC_type::VECTOR );
}

TEST_CASE( "collapse recurses into map values", "[collapse]" )
{
  GenericContainer gc;
  auto &           v = gc["nums"].set_vector( 2 );
  v[0] = 5;
  v[1] = 6;
  gc["other"] = "text";
  gc.collapse();
  CHECK( gc.get_type() == GC_type::MAP );
  CHECK( std::as_const( gc )( "nums" ).get_type() == GC_type::VEC_INTEGER );
  CHECK( std::as_const( gc )( "nums" ).get_vec_int() == vec_int_type{ 5, 6 } );
}
