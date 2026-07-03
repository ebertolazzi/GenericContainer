/*--------------------------------------------------------------------------*\
 |  Characterization tests: the FFI wrapper (opaque handles + JSON).        |
 |  Its C ABI is frozen through the modernization.                          |
\*--------------------------------------------------------------------------*/

#include "GenericContainer/GenericContainer_ffi.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

TEST_CASE( "FFI scalar round trips through a handle", "[ffi]" )
{
  gc_handle_t h = gc_new();
  REQUIRE( h != nullptr );

  gc_set_bool( h, 1 );
  CHECK( gc_get_bool( h ) == 1 );

  gc_set_int32( h, -7 );
  CHECK( gc_get_int32( h ) == -7 );

  gc_set_int64( h, 1LL << 40 );
  CHECK( gc_get_int64( h ) == ( 1LL << 40 ) );

  gc_set_real( h, 2.5 );
  CHECK( gc_get_real( h ) == 2.5 );

  gc_set_string( h, "ffi" );
  CHECK( std::string( gc_get_string( h ) ) == "ffi" );

  gc_free( h );
}

TEST_CASE( "FFI json round trip", "[ffi]" )
{
  gc_handle_t h = gc_new();
  REQUIRE( h != nullptr );

  char const * doc = R"({"a": 1, "b": [1.5, 2.5], "c": {"nested": true}})";
  REQUIRE( gc_from_json( h, doc ) == 0 );

  char * out = gc_to_json( h );
  REQUIRE( out != nullptr );
  std::string const round( out );
  gc_free_string( out );

  gc_handle_t h2 = gc_new();
  REQUIRE( gc_from_json( h2, round.c_str() ) == 0 );
  char * out2 = gc_to_json( h2 );
  REQUIRE( out2 != nullptr );
  CHECK( round == out2 );  // stable after one round trip
  gc_free_string( out2 );

  gc_free( h2 );
  gc_free( h );
}

TEST_CASE( "FFI reports errors through gc_last_error", "[ffi]" )
{
  gc_handle_t h = gc_new();
  REQUIRE( h != nullptr );

  CHECK( gc_from_json( h, "{ not valid json" ) != 0 );
  CHECK( gc_last_error() != nullptr );
  CHECK( std::string( gc_last_error() ) != "" );

  gc_free( h );
}

TEST_CASE( "FFI null handles are safe", "[ffi]" )
{
  CHECK( gc_from_json( nullptr, "{}" ) != 0 );
  CHECK( gc_to_json( nullptr ) == nullptr );
  CHECK_NOTHROW( gc_set_int32( nullptr, 1 ) );
  CHECK_NOTHROW( gc_free( nullptr ) );
  CHECK_NOTHROW( gc_free_string( nullptr ) );
}
