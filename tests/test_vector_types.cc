/*--------------------------------------------------------------------------*\
 |  Characterization tests: the seven vector types and push_* semantics.    |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <utility>

using namespace GC_namespace;

TEST_CASE( "sized set_vec_* allocate default-initialized vectors", "[vector]" )
{
  GenericContainer gc;

  auto & vb = gc.set_vec_bool( 3 );
  CHECK( gc.get_type() == GC_type::VEC_BOOL );
  CHECK( vb.size() == 3 );

  auto & vi = gc.set_vec_int( 4 );
  CHECK( gc.get_type() == GC_type::VEC_INTEGER );
  CHECK( vi.size() == 4 );

  auto & vl = gc.set_vec_long( 2 );
  CHECK( gc.get_type() == GC_type::VEC_LONG );
  CHECK( vl.size() == 2 );

  auto & vr = gc.set_vec_real( 5 );
  CHECK( gc.get_type() == GC_type::VEC_REAL );
  CHECK( vr.size() == 5 );

  auto & vc = gc.set_vec_complex( 2 );
  CHECK( gc.get_type() == GC_type::VEC_COMPLEX );
  CHECK( vc.size() == 2 );

  auto & vs = gc.set_vec_string( 3 );
  CHECK( gc.get_type() == GC_type::VEC_STRING );
  CHECK( vs.size() == 3 );

  auto & vp = gc.set_vec_pointer( 2 );
  CHECK( gc.get_type() == GC_type::VEC_POINTER );
  CHECK( vp.size() == 2 );
}

TEST_CASE( "set_vec_* from existing vector deep-copies", "[vector]" )
{
  GenericContainer gc;

  vec_int_type src{ 1, -2, 3 };
  gc.set_vec_int( src );
  src[0] = 99;  // source mutation must not affect stored copy
  CHECK( std::as_const( gc ).get_vec_int() == vec_int_type{ 1, -2, 3 } );

  vec_string_type ssrc{ "a", "b" };
  gc.set_vec_string( ssrc );
  CHECK( std::as_const( gc ).get_vec_string() == ssrc );

  vec_complex_type csrc{ complex_type( 1, 2 ), complex_type( 3, 4 ) };
  gc.set_vec_complex( csrc );
  CHECK( std::as_const( gc ).get_vec_complex() == csrc );

  vec_bool_type bsrc{ true, false };
  gc.set_vec_bool( bsrc );
  CHECK( std::as_const( gc ).get_vec_bool() == bsrc );

  vec_long_type lsrc{ 1LL << 40 };
  gc.set_vec_long( lsrc );
  CHECK( std::as_const( gc ).get_vec_long() == lsrc );

  vec_real_type rsrc{ 0.5, -1.5 };
  gc.set_vec_real( rsrc );
  CHECK( std::as_const( gc ).get_vec_real() == rsrc );
}

TEST_CASE( "vector assignment operators", "[vector]" )
{
  GenericContainer gc;
  gc = vec_int_type{ 4, 5 };
  CHECK( gc.get_type() == GC_type::VEC_INTEGER );
  CHECK( std::as_const( gc ).get_vec_int() == vec_int_type{ 4, 5 } );

  gc = vec_real_type{ 1.5 };
  CHECK( gc.get_type() == GC_type::VEC_REAL );

  gc = vec_string_type{ "x" };
  CHECK( gc.get_type() == GC_type::VEC_STRING );

  gc = vec_bool_type{ false, true };
  CHECK( gc.get_type() == GC_type::VEC_BOOL );

  gc = vec_long_type{ 2 };
  CHECK( gc.get_type() == GC_type::VEC_LONG );

  gc = vec_complex_type{ complex_type( 0, 1 ) };
  CHECK( gc.get_type() == GC_type::VEC_COMPLEX );
}

TEST_CASE( "get_*_at element access on vectors", "[vector]" )
{
  GenericContainer gc;
  gc = vec_int_type{ 10, 20, 30 };
  CHECK( gc.get_int_at( 1 ) == 20 );
  CHECK( std::as_const( gc ).get_int_at( 2, "test" ) == 30 );

  gc = vec_real_type{ 0.5, 1.5 };
  CHECK( gc.get_real_at( 0 ) == 0.5 );
  CHECK( std::as_const( gc ).get_real_at( 1, "test" ) == 1.5 );

  gc = vec_bool_type{ true, false };
  CHECK( gc.get_bool_at( 0 ) == true );
  CHECK( std::as_const( gc ).get_bool_at( 1, "test" ) == false );

  gc = vec_string_type{ "aa", "bb" };
  CHECK( gc.get_string_at( 1 ) == "bb" );
  CHECK( std::as_const( gc ).get_string_at( 0, "test" ) == "aa" );

  gc = vec_complex_type{ complex_type( 1, 1 ) };
  CHECK( std::as_const( gc ).get_complex_at( 0, "test" ) == complex_type( 1, 1 ) );

  gc = vec_long_type{ 7, 8 };
  CHECK( gc.get_long_at( 1 ) == 8 );
  CHECK( std::as_const( gc ).get_long_at( 0, "test" ) == 7 );
}

TEST_CASE( "get_number_at / get_complex_number_at convert per element", "[vector]" )
{
  GenericContainer gc;
  gc = vec_int_type{ 3, 4 };
  CHECK( gc.get_number_at( 0 ) == 3.0 );

  gc = vec_real_type{ 2.5 };
  CHECK( gc.get_number_at( 0 ) == 2.5 );
  CHECK( gc.get_complex_number_at( 0 ) == complex_type( 2.5, 0 ) );

  gc = vec_complex_type{ complex_type( 1, -1 ) };
  CHECK( gc.get_complex_number_at( 0 ) == complex_type( 1, -1 ) );

  real_type re{ 0 }, im{ 0 };
  gc.get_complex_number_at( 0, re, im );
  CHECK( re == 1.0 );
  CHECK( im == -1.0 );
}

TEST_CASE( "push_* grows matching vector type", "[vector][push]" )
{
  GenericContainer gc;
  gc.set_vec_int( 0 );
  gc.push_int( 1 );
  gc.push_int( 2 );
  CHECK( std::as_const( gc ).get_vec_int() == vec_int_type{ 1, 2 } );

  gc.set_vec_real( 0 );
  gc.push_real( 0.5 );
  CHECK( std::as_const( gc ).get_vec_real() == vec_real_type{ 0.5 } );

  gc.set_vec_string( 0 );
  gc.push_string( "s1" );
  gc.push_string( "s2" );
  CHECK( std::as_const( gc ).get_vec_string() == vec_string_type{ "s1", "s2" } );

  gc.set_vec_bool( 0 );
  gc.push_bool( true );
  CHECK( std::as_const( gc ).get_vec_bool() == vec_bool_type{ true } );

  gc.set_vec_long( 0 );
  gc.push_long( 9 );
  CHECK( std::as_const( gc ).get_vec_long() == vec_long_type{ 9 } );

  gc.set_vec_complex( 0 );
  gc.push_complex( 1.0, 2.0 );
  CHECK( std::as_const( gc ).get_vec_complex() == vec_complex_type{ complex_type( 1, 2 ) } );
}

TEST_CASE( "push into wider vector promotes the value", "[vector][push]" )
{
  // bool pushed into an integer vector becomes 0/1, etc.
  GenericContainer gc;
  gc.set_vec_int( 0 );
  gc.push_int( 5 );
  gc.push_bool( true );
  CHECK( std::as_const( gc ).get_vec_int() == vec_int_type{ 5, 1 } );

  gc.set_vec_real( 0 );
  gc.push_real( 1.5 );
  gc.push_bool( false );
  CHECK( std::as_const( gc ).get_vec_real() == vec_real_type{ 1.5, 0.0 } );
}

TEST_CASE( "push of wider value promotes the vector type", "[vector][push]" )
{
  // Characterization: pushing a real into a vec_int promotes storage to vec_real.
  GenericContainer gc;
  gc.set_vec_int( 0 );
  gc.push_int( 2 );
  gc.push_real( 0.5 );
  CHECK( gc.get_type() == GC_type::VEC_REAL );
  CHECK( std::as_const( gc ).get_vec_real() == vec_real_type{ 2.0, 0.5 } );

  gc.set_vec_bool( 0 );
  gc.push_bool( true );
  gc.push_int( 3 );
  CHECK( gc.get_type() == GC_type::VEC_INTEGER );
  CHECK( std::as_const( gc ).get_vec_int() == vec_int_type{ 1, 3 } );
}

TEST_CASE( "push into VECTOR appends a generic element", "[vector][push]" )
{
  GenericContainer gc;
  gc.set_vector( 0 );
  gc.push_int( 11 );
  gc.push_string( "s" );
  CHECK( gc.get_type() == GC_type::VECTOR );
  CHECK( gc.get_num_elements() == 2 );
  CHECK( std::as_const( gc )( 0 ).get_int() == 11 );
  CHECK( std::as_const( gc )( 1 ).get_string() == "s" );
}

TEST_CASE( "vec_bool proxy access works through get_bool_at", "[vector][vecbool]" )
{
  GenericContainer gc;
  auto &           vb = gc.set_vec_bool( 4 );
  vb[2]               = true;
  CHECK( gc.get_bool_at( 2 ) == true );
  CHECK( gc.get_bool_at( 0 ) == false );
  CHECK( gc.get_num_elements() == 4 );
}

TEST_CASE( "wrong-type vector get throws", "[vector][errors]" )
{
  GenericContainer gc;
  gc = vec_int_type{ 1 };
  GenericContainer const & cgc = gc;
  CHECK_THROWS_AS( cgc.get_vec_real(), std::runtime_error );
  CHECK_THROWS_AS( cgc.get_vec_string(), std::runtime_error );
  CHECK_THROWS_AS( cgc.get_vector(), std::runtime_error );
}
