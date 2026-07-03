/*--------------------------------------------------------------------------*\
 |  Characterization tests: the promotion matrix.                           |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <utility>

using namespace GC_namespace;

TEST_CASE( "scalar promotion ladder bool->int->long / ->real->complex", "[promote]" )
{
  GenericContainer gc;

  gc.set_bool( true );
  gc.promote_to_int();
  CHECK( gc.get_type() == GC_type::INTEGER );
  CHECK( std::as_const( gc ).get_int() == 1 );

  gc.promote_to_long();
  CHECK( gc.get_type() == GC_type::LONG );
  CHECK( std::as_const( gc ).get_long() == 1 );

  gc.set_int( -3 );
  gc.promote_to_real();
  CHECK( gc.get_type() == GC_type::REAL );
  CHECK( std::as_const( gc ).get_real() == -3.0 );

  gc.promote_to_complex();
  CHECK( gc.get_type() == GC_type::COMPLEX );
  CHECK( std::as_const( gc ).get_complex() == complex_type( -3.0, 0.0 ) );
}

TEST_CASE( "promotion is idempotent on the target type", "[promote]" )
{
  GenericContainer gc;
  gc.set_real( 2.5 );
  gc.promote_to_real();
  CHECK( std::as_const( gc ).get_real() == 2.5 );

  gc = vec_int_type{ 1, 2 };
  gc.promote_to_vec_int();
  CHECK( std::as_const( gc ).get_vec_int() == vec_int_type{ 1, 2 } );
}

TEST_CASE( "promotion from NOTYPE yields zero-valued singleton", "[promote]" )
{
  GenericContainer gc;
  gc.promote_to_int();
  CHECK( std::as_const( gc ).get_int() == 0 );

  gc.clear();
  gc.promote_to_vec_int();
  CHECK( gc.get_type() == GC_type::VEC_INTEGER );
  CHECK( std::as_const( gc ).get_vec_int() == vec_int_type{ 0 } );

  gc.clear();
  gc.promote_to_mat_real();
  CHECK( gc.get_type() == GC_type::MAT_REAL );
  CHECK( gc.num_rows() == 1 );
  CHECK( gc.num_cols() == 1 );
}

TEST_CASE( "illegal scalar promotions throw", "[promote][errors]" )
{
  GenericContainer gc;
  gc.set_real( 1.5 );
  CHECK_THROWS_AS( gc.promote_to_int(), std::runtime_error );
  CHECK_THROWS_AS( gc.promote_to_long(), std::runtime_error );

  gc.set_string( "s" );
  CHECK_THROWS_AS( gc.promote_to_real(), std::runtime_error );
  CHECK_THROWS_AS( gc.promote_to_complex(), std::runtime_error );
}

TEST_CASE( "scalar to vec promotions wrap into singleton", "[promote]" )
{
  GenericContainer gc;
  gc.set_int( 7 );
  gc.promote_to_vec_int();
  CHECK( std::as_const( gc ).get_vec_int() == vec_int_type{ 7 } );

  gc.set_long( 8 );
  gc.promote_to_vec_long();
  CHECK( std::as_const( gc ).get_vec_long() == vec_long_type{ 8 } );

  gc.set_real( 0.5 );
  gc.promote_to_vec_real();
  CHECK( std::as_const( gc ).get_vec_real() == vec_real_type{ 0.5 } );

  gc.set_complex( 1, 2 );
  gc.promote_to_vec_complex();
  CHECK( std::as_const( gc ).get_vec_complex() == vec_complex_type{ complex_type( 1, 2 ) } );
}

TEST_CASE( "vec_bool -> vec_int -> vec_real -> vec_complex widening", "[promote]" )
{
  GenericContainer gc;
  gc = vec_bool_type{ true, false, true };
  gc.promote_to_vec_int();
  CHECK( std::as_const( gc ).get_vec_int() == vec_int_type{ 1, 0, 1 } );

  gc.promote_to_vec_real();
  CHECK( std::as_const( gc ).get_vec_real() == vec_real_type{ 1.0, 0.0, 1.0 } );

  gc.promote_to_vec_complex();
  CHECK(
    std::as_const( gc ).get_vec_complex() ==
    vec_complex_type{ complex_type( 1, 0 ), complex_type( 0, 0 ), complex_type( 1, 0 ) } );
}

TEST_CASE( "vec_long promotes to vec_real but vec_int does not promote to vec_int from long", "[promote][quirk]" )
{
  GenericContainer gc;
  gc = vec_long_type{ 5, 6 };
  gc.promote_to_vec_real();
  CHECK( std::as_const( gc ).get_vec_real() == vec_real_type{ 5.0, 6.0 } );

  // Characterization quirk: a LONG scalar cannot be promoted to vec_int...
  gc.set_long( 5 );
  CHECK_THROWS_AS( gc.promote_to_vec_int(), std::runtime_error );
  // ...nor to mat_real (while INTEGER can).
  gc.set_long( 5 );
  CHECK_THROWS_AS( gc.promote_to_mat_real(), std::runtime_error );
}

TEST_CASE( "vec_real to mat_real promotion produces a column", "[promote]" )
{
  GenericContainer gc;
  gc = vec_real_type{ 1.0, 2.0, 3.0 };
  gc.promote_to_mat_real();
  CHECK( gc.get_type() == GC_type::MAT_REAL );
  CHECK( gc.num_rows() == 3 );
  CHECK( gc.num_cols() == 1 );
  CHECK( std::as_const( gc ).get_mat_real()( 2, 0 ) == 3.0 );
}

TEST_CASE( "vec to mat_int/long/complex promotions fill the column", "[promote][newbehavior]" )
{
  // BUG (pre-rewrite): promote_to_mat_int/mat_long/mat_complex write elements
  // through m_data.m_r (matrix-of-real) while the union holds a different
  // matrix type -- copy-paste type confusion, UB. Only promote_to_mat_real is
  // correct. Fixed structurally by the Phase 2 variant rewrite.
  GenericContainer gc;
  gc = vec_int_type{ 4, 5 };
  gc.promote_to_mat_int();
  CHECK( gc.num_rows() == 2 );
  CHECK( gc.num_cols() == 1 );
  CHECK( std::as_const( gc ).get_mat_int()( 1, 0 ) == 5 );

  gc = vec_bool_type{ true, false };
  gc.promote_to_mat_long();
  CHECK( std::as_const( gc ).get_mat_long()( 0, 0 ) == 1 );

  gc = vec_real_type{ 1.5, 2.5 };
  gc.promote_to_mat_complex();
  CHECK( std::as_const( gc ).get_mat_complex()( 1, 0 ) == complex_type( 2.5, 0 ) );
}

TEST_CASE( "mat_int promotes to mat_long and mat_real keeping shape", "[promote]" )
{
  GenericContainer gc;
  auto &           m = gc.set_mat_int( 2, 3 );
  for ( unsigned j{ 0 }; j < 3; ++j )
    for ( unsigned i{ 0 }; i < 2; ++i ) m( i, j ) = int_type( 10 * i + j );

  gc.promote_to_mat_long();
  CHECK( gc.get_type() == GC_type::MAT_LONG );
  CHECK( gc.num_rows() == 2 );
  CHECK( gc.num_cols() == 3 );
  CHECK( std::as_const( gc ).get_mat_long()( 1, 2 ) == 12 );

  gc.promote_to_mat_real();
  CHECK( gc.get_type() == GC_type::MAT_REAL );
  CHECK( std::as_const( gc ).get_mat_real()( 1, 2 ) == 12.0 );

  gc.promote_to_mat_complex();
  CHECK( gc.get_type() == GC_type::MAT_COMPLEX );
  CHECK( std::as_const( gc ).get_mat_complex()( 1, 2 ) == complex_type( 12.0, 0.0 ) );
}

TEST_CASE( "promote_to_vector wraps typed vectors element-wise", "[promote]" )
{
  GenericContainer gc;
  gc = vec_int_type{ 3, 4 };
  gc.promote_to_vector();
  CHECK( gc.get_type() == GC_type::VECTOR );
  CHECK( gc.get_num_elements() == 2 );
  CHECK( std::as_const( gc )( 1 ).get_int() == 4 );
}

TEST_CASE( "promote_to_vector wraps scalars into one-element VECTOR", "[promote][newbehavior]" )
{
  // BUG (pre-rewrite): the scalar cases of promote_to_vector do
  //   set_vector( 1 ); ( *this )[0] = m_data.i;
  // reading the union member AFTER set_vector repurposed it as the vector
  // pointer -- garbage for arithmetic scalars, use-after-free for
  // STRING/COMPLEX. The VEC_* cases save the pointer first and are fine.
  // Fixed structurally by the Phase 2 variant rewrite.
  GenericContainer gc;
  gc.set_int( 3 );
  gc.promote_to_vector();
  CHECK( gc.get_type() == GC_type::VECTOR );
  CHECK( gc.get_num_elements() == 1 );
  CHECK( std::as_const( gc )( 0 ).get_int() == 3 );

  gc.set_string( "s" );
  gc.promote_to_vector();
  CHECK( gc.get_type() == GC_type::VECTOR );
  CHECK( std::as_const( gc )( 0 ).get_string() == "s" );
}

TEST_CASE( "copyto_vec_* convert element-wise across source types", "[promote][copyto]" )
{
  GenericContainer gc;
  gc = vec_bool_type{ true, false };

  vec_int_type vi;
  gc.copyto_vec_int( vi );
  CHECK( vi == vec_int_type{ 1, 0 } );

  gc = vec_int_type{ 3, -4 };
  vec_real_type vr;
  gc.copyto_vec_real( vr );
  CHECK( vr == vec_real_type{ 3.0, -4.0 } );

  vec_long_type vl;
  gc.copyto_vec_long( vl );
  CHECK( vl == vec_long_type{ 3, -4 } );

  vec_complex_type vc;
  gc.copyto_vec_complex( vc );
  CHECK( vc == vec_complex_type{ complex_type( 3, 0 ), complex_type( -4, 0 ) } );

  // scalar source becomes 1-element vector
  gc.set_real( 9.0 );
  gc.copyto_vec_real( vr );
  CHECK( vr == vec_real_type{ 9.0 } );

  // integral-valued reals convert to int
  gc = vec_real_type{ 2.0, 4.0 };
  gc.copyto_vec_int( vi );
  CHECK( vi == vec_int_type{ 2, 4 } );

  // fractional reals must be rejected
  gc = vec_real_type{ 2.5 };
  CHECK_THROWS_AS( gc.copyto_vec_int( vi ), std::runtime_error );
  CHECK_THROWS_AS( gc.copyto_vec_long( vl ), std::runtime_error );

  // negative values rejected for unsigned targets
  gc = vec_int_type{ -1 };
  vec_uint_type vu;
  CHECK_THROWS_AS( gc.copyto_vec_uint( vu ), std::runtime_error );
  vec_ulong_type vul;
  CHECK_THROWS_AS( gc.copyto_vec_ulong( vul ), std::runtime_error );

  // strings copy to strings
  gc = vec_string_type{ "x", "y" };
  vec_string_type vs;
  gc.copyto_vec_string( vs );
  CHECK( vs == vec_string_type{ "x", "y" } );
}
