/*--------------------------------------------------------------------------*\
 |  Characterization tests: the extern "C" interface. Its ABI is frozen     |
 |  through the modernization; these tests keep it honest.                  |
\*--------------------------------------------------------------------------*/

#include "GenericContainer/GenericContainerInterface_C.h"

#include <catch2/catch_test_macros.hpp>

#include <cstring>
#include <string>

namespace
{
  //! RAII for the global C-side container registry.
  struct CGuard
  {
    std::string id;
    explicit CGuard( char const * name ) : id( name ) { REQUIRE( GC_new( id.c_str() ) == 0 ); }
    ~CGuard() { GC_delete( id.c_str() ); }
  };
}  // namespace

TEST_CASE( "C API scalar set/get round trips", "[c_api]" )
{
  CGuard g( "c_scalars" );

  CHECK( GC_set_bool( 1 ) == 0 );
  CHECK( GC_get_bool() == 1 );

  CHECK( GC_set_int( -12 ) == 0 );
  CHECK( GC_get_int() == -12 );

  CHECK( GC_set_real( 2.5 ) == 0 );
  CHECK( GC_get_real() == 2.5 );

  CHECK( GC_set_complex2( 1.0, -2.0 ) == 0 );
  CHECK( GC_get_complex_re() == 1.0 );
  CHECK( GC_get_complex_im() == -2.0 );

  CHECK( GC_set_string( "from C" ) == 0 );
  CHECK( std::string( GC_get_string() ) == "from C" );
}

TEST_CASE( "C API vector set and element access", "[c_api]" )
{
  CGuard g( "c_vectors" );

  int const ivals[3]{ 4, 5, 6 };
  CHECK( GC_set_vector_of_int( ivals, 3 ) == 0 );
  CHECK( GC_get_vector_size() == 3 );
  CHECK( GC_get_int_at_pos( 1 ) == 5 );

  double const rvals[2]{ 0.5, 1.5 };
  CHECK( GC_set_vector_of_real( rvals, 2 ) == 0 );
  CHECK( GC_get_real_at_pos( 1 ) == 1.5 );

  char const * svals[2]{ "aa", "bb" };
  CHECK( GC_set_vector_of_string( svals, 2 ) == 0 );
  CHECK( std::string( GC_get_string_at_pos( 0 ) ) == "aa" );
}

TEST_CASE( "C API push into generic vector", "[c_api]" )
{
  CGuard g( "c_push" );

  CHECK( GC_set_empty_vector() == 0 );
  CHECK( GC_push_int( 3 ) == 0 );
  CHECK( GC_push_string( "el" ) == 0 );
  CHECK( GC_get_vector_size() == 2 );
}

TEST_CASE( "C API map navigation with keys", "[c_api]" )
{
  CGuard g( "c_map" );

  CHECK( GC_set_map() == 0 );
  CHECK( GC_push_map_position( "sub" ) == 0 );
  CHECK( GC_set_int( 9 ) == 0 );
  CHECK( GC_pop_head() == 0 );

  CHECK( GC_init_map_key() == 0 );
  char const * key = GC_get_next_key();
  REQUIRE( key != nullptr );
  CHECK( std::string( key ) == "sub" );
  // exhausted iteration yields an empty string, not NULL
  char const * done = GC_get_next_key();
  REQUIRE( done != nullptr );
  CHECK( std::string( done ).empty() );
}

TEST_CASE( "C API select switches between containers", "[c_api]" )
{
  CGuard a( "c_sel_a" );
  CGuard b( "c_sel_b" );

  REQUIRE( GC_select( "c_sel_a" ) == 0 );
  GC_set_int( 1 );
  REQUIRE( GC_select( "c_sel_b" ) == 0 );
  GC_set_int( 2 );

  REQUIRE( GC_select( "c_sel_a" ) == 0 );
  CHECK( GC_get_int() == 1 );
  REQUIRE( GC_select( "c_sel_b" ) == 0 );
  CHECK( GC_get_int() == 2 );
}

TEST_CASE( "C API error codes on misuse", "[c_api]" )
{
  // Characterization: GC_select auto-creates unknown ids (returns 0 and
  // registers a fresh empty container).
  CHECK( GC_select( "auto_created_by_select" ) == 0 );
  CHECK( GC_get_type() == 0 );  // NOTYPE
  GC_delete( "auto_created_by_select" );

  CGuard g( "c_err" );
  GC_set_int( 5 );
  // pushing into a scalar is refused with a nonzero code (C API maps
  // exceptions to return codes; it must not throw across the C boundary)
  CHECK( GC_push_map_position( "k" ) != 0 );
}
