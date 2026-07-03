/*--------------------------------------------------------------------------*\
 |  Characterization tests: built-in from/to json, yaml, toml interfaces.   |
 |  These keep the interface layers honest through the core rewrite.        |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>

#include <sstream>

using namespace GC_namespace;

TEST_CASE( "from_json / to_json round trip", "[interfaces][json]" )
{
  GenericContainer gc;
  REQUIRE( gc.from_json( std::string( R"({"a": 1, "b": [1.5, 2.5], "c": {"d": true}})" ) ) );
  CHECK( gc.get_type() == GC_type::MAP );
  CHECK( std::as_const( gc )( "a" ).get_number() == 1.0 );
  CHECK( std::as_const( gc )( "c" )( "d" ).get_bool() == true );

  std::string const text = gc.to_json();
  CHECK_FALSE( text.empty() );

  GenericContainer back;
  std::istringstream in( text );
  REQUIRE( back.from_json( in ) );
  CHECK( back.get_type() == GC_type::MAP );
  CHECK( std::as_const( back )( "c" )( "d" ).get_bool() == true );
}

TEST_CASE( "from_yaml / to_yaml round trip", "[interfaces][yaml]" )
{
  GenericContainer gc;
  REQUIRE( gc.from_yaml( std::string( "a: 1\nb:\n  - 1.5\n  - 2.5\nc:\n  d: true\n" ) ) );
  CHECK( gc.get_type() == GC_type::MAP );
  CHECK( std::as_const( gc )( "c" )( "d" ).get_bool() == true );

  std::string const text = gc.to_yaml();
  CHECK_FALSE( text.empty() );

  GenericContainer back;
  REQUIRE( back.from_yaml( text ) );
  CHECK( back.get_type() == GC_type::MAP );
  CHECK( std::as_const( back )( "c" )( "d" ).get_bool() == true );
}

TEST_CASE( "from_toml / to_toml round trip", "[interfaces][toml]" )
{
  GenericContainer gc;
  REQUIRE( gc.from_toml( std::string( "a = 1\nb = [1.5, 2.5]\n[c]\nd = true\n" ) ) );
  CHECK( gc.get_type() == GC_type::MAP );
  CHECK( std::as_const( gc )( "c" )( "d" ).get_bool() == true );

  std::string const text = gc.to_toml();
  CHECK_FALSE( text.empty() );

  GenericContainer back;
  REQUIRE( back.from_toml( text ) );
  CHECK( std::as_const( back )( "c" )( "d" ).get_bool() == true );
}

TEST_CASE( "fixture files parse through each interface", "[interfaces]" )
{
  GenericContainer gj;
  CHECK( gj.from_file( "examples/data.json" ) );
  CHECK( gj.get_type() == GC_type::MAP );

  GenericContainer gy;
  CHECK( gy.from_file( "examples/test1.yml" ) );
  CHECK_FALSE( gy.empty() );

  GenericContainer gt;
  CHECK( gt.from_file( "examples/settings.toml" ) );
  CHECK( gt.get_type() == GC_type::MAP );
}

TEST_CASE( "matrix survives yaml and json round trips as nested arrays", "[interfaces]" )
{
  GenericContainer gc;
  auto &           m = gc["M"].set_mat_real( 2, 2 );
  m( 0, 0 ) = 1.0;
  m( 0, 1 ) = 2.0;
  m( 1, 0 ) = 3.0;
  m( 1, 1 ) = 4.0;

  GenericContainer back;
  REQUIRE( back.from_json( gc.to_json() ) );
  CHECK( back.exists( "M" ) );

  GenericContainer backy;
  REQUIRE( backy.from_yaml( gc.to_yaml() ) );
  CHECK( backy.exists( "M" ) );
}
