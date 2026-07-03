/*--------------------------------------------------------------------------*\
 |  Characterization tests: matrix types. These pin GC-level behavior       |
 |  (accessors, layout, dimensions) that must survive the Eigen migration;  |
 |  mat_type member API details change in Phase 3.                          |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <utility>

using namespace GC_namespace;

namespace
{
  //! Fill a matrix with a value derived from (i,j) so layout mistakes surface.
  template <typename MAT> void fill_ij( MAT & m, unsigned nr, unsigned nc )
  {
    for ( unsigned j{ 0 }; j < nc; ++j )
      for ( unsigned i{ 0 }; i < nr; ++i ) m( i, j ) = static_cast<typename MAT::value_type>( 100 * i + j );
  }
}  // namespace

TEST_CASE( "set_mat_* allocate with dimensions", "[matrix]" )
{
  GenericContainer gc;

  gc.set_mat_int( 2, 3 );
  CHECK( gc.get_type() == GC_type::MAT_INTEGER );
  CHECK( gc.num_rows() == 2 );
  CHECK( gc.num_cols() == 3 );
  CHECK( gc.get_num_elements() == 6 );

  gc.set_mat_long( 1, 4 );
  CHECK( gc.get_type() == GC_type::MAT_LONG );
  CHECK( gc.num_rows() == 1 );
  CHECK( gc.num_cols() == 4 );

  gc.set_mat_real( 3, 3 );
  CHECK( gc.get_type() == GC_type::MAT_REAL );
  CHECK( gc.num_rows() == 3 );

  gc.set_mat_complex( 2, 2 );
  CHECK( gc.get_type() == GC_type::MAT_COMPLEX );
  CHECK( gc.num_cols() == 2 );
}

TEST_CASE( "matrix element read/write via get_*_at(i,j)", "[matrix]" )
{
  GenericContainer gc;
  gc.set_mat_real( 2, 3 );
  gc.get_real_at( 1, 2 ) = 42.5;
  CHECK( std::as_const( gc ).get_real_at( 1, 2, "test" ) == 42.5 );
  CHECK( std::as_const( gc ).get_real_at( 0, 0, "test" ) == 0.0 );

  gc.set_mat_int( 2, 2 );
  gc.get_int_at( 0, 1 ) = 7;
  CHECK( std::as_const( gc ).get_int_at( 0, 1, "test" ) == 7 );

  gc.set_mat_long( 2, 2 );
  gc.get_long_at( 1, 1 ) = 1LL << 41;
  CHECK( std::as_const( gc ).get_long_at( 1, 1, "test" ) == ( 1LL << 41 ) );

  gc.set_mat_complex( 2, 2 );
  gc.get_complex_at( 1, 0 ) = complex_type( 2, -3 );
  CHECK( std::as_const( gc ).get_complex_at( 1, 0, "test" ) == complex_type( 2, -3 ) );
}

TEST_CASE( "matrix storage is column-major", "[matrix][layout]" )
{
  // The serialization wire format depends on this layout; it must survive
  // the Eigen migration (Eigen's default is also column-major).
  GenericContainer gc;
  auto &           m = gc.set_mat_real( 2, 3 );
  fill_ij( m, 2, 3 );
  // element (i,j) lives at data[i + j*num_rows]
  real_type const * data = m.data();
  CHECK( data[0] == 0.0 );    // (0,0)
  CHECK( data[1] == 100.0 );  // (1,0)
  CHECK( data[2] == 1.0 );    // (0,1)
  CHECK( data[3] == 101.0 );  // (1,1)
  CHECK( data[4] == 2.0 );    // (0,2)
  CHECK( data[5] == 102.0 );  // (1,2)
}

TEST_CASE( "set_mat_* from existing matrix deep-copies", "[matrix]" )
{
  mat_real_type src( 2, 2 );
  src( 0, 0 ) = 1.0;
  src( 1, 1 ) = 4.0;

  GenericContainer gc;
  gc.set_mat_real( src );
  src( 0, 0 ) = 99.0;
  CHECK( std::as_const( gc ).get_mat_real()( 0, 0 ) == 1.0 );
  CHECK( std::as_const( gc ).get_mat_real()( 1, 1 ) == 4.0 );
}

TEST_CASE( "matrix assignment operator", "[matrix]" )
{
  mat_int_type m( 2, 2 );
  m( 0, 1 ) = 5;

  GenericContainer gc;
  gc = m;
  CHECK( gc.get_type() == GC_type::MAT_INTEGER );
  CHECK( std::as_const( gc ).get_mat_int()( 0, 1 ) == 5 );
}

TEST_CASE( "get_row and get_column copy out slices", "[matrix]" )
{
  GenericContainer gc;
  auto &           m = gc.set_mat_real( 2, 3 );
  fill_ij( m, 2, 3 );

  vec_real_type col;
  m.get_column( 1, col );
  CHECK( col == vec_real_type{ 1.0, 101.0 } );

  vec_real_type row;
  m.get_row( 1, row );
  CHECK( row == vec_real_type{ 100.0, 101.0, 102.0 } );
}

TEST_CASE( "num_rows/num_cols conventions for non-matrix types", "[matrix]" )
{
  GenericContainer gc;
  gc = vec_real_type{ 1.0, 2.0, 3.0 };
  CHECK( gc.num_rows() == 1 );
  CHECK( gc.num_cols() == 3 );

  gc.set_int( 4 );
  CHECK( gc.num_rows() == 1 );
  CHECK( gc.num_cols() == 1 );

  gc.clear();
  CHECK( gc.num_rows() == 0 );
  CHECK( gc.num_cols() == 0 );
}

TEST_CASE( "wrong-type matrix get throws", "[matrix][errors]" )
{
  GenericContainer gc;
  gc.set_mat_real( 2, 2 );
  GenericContainer const & cgc = gc;
  CHECK_THROWS_AS( cgc.get_mat_int(), std::runtime_error );
  CHECK_THROWS_AS( cgc.get_vec_real(), std::runtime_error );
}

TEST_CASE( "out-of-range matrix access via checked accessor throws", "[matrix][errors][newbehavior]" )
{
  // Bounds-checked get_*_at is guaranteed behavior after the Eigen migration
  // (Phase 3); the pre-rewrite mat_type::operator() relies on vector::at with
  // the same observable effect, so this runs unskipped.
  GenericContainer gc;
  gc.set_mat_real( 2, 2 );
  GenericContainer const & cgc = gc;
  CHECK_THROWS_AS( cgc.get_real_at( 5, 5, "test" ), std::runtime_error );
}
