/*--------------------------------------------------------------------------*\
 |  Characterization tests: built-in from/to json, yaml, toml interfaces.   |
 |  These keep the interface layers honest through the core rewrite.        |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>

using namespace GC_namespace;

namespace
{
  static std::filesystem::path
  fixture_path( char const * name )
  {
    return std::filesystem::path( __FILE__ ).parent_path().parent_path() / "examples" / name;
  }
}

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

TEST_CASE( "from_yaml handles nested mappings through fkYAML map_items", "[interfaces][yaml]" )
{
  GenericContainer gc;
  REQUIRE( gc.from_yaml(
    std::string(
      "root:\n"
      "  nested:\n"
      "    answer: 42\n"
      "    enabled: true\n"
      "  list:\n"
      "    - name: alpha\n"
      "      value: 1\n"
      "    - name: beta\n"
      "      value: 2\n"
    )
  ) );

  auto const & root   = std::as_const( gc )( "root" );
  auto const & nested = root( "nested" );
  CHECK( nested( "answer" ).get_int() == 42 );
  CHECK( nested( "enabled" ).get_bool() == true );
  CHECK( root( "list" ).get_type() == GC_type::VECTOR );
  CHECK( root( "list" )[0]( "name" ).get_string() == "alpha" );
  CHECK( root( "list" )[1]( "value" ).get_int() == 2 );
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

TEST_CASE( "gc_from_json/yaml/toml static helpers populate containers", "[interfaces]" )
{
  auto const gj = GenericContainer::gc_from_json( R"({"id": 7, "name": "json"})" );
  CHECK( gj.get_type() == GC_type::MAP );
  CHECK( std::as_const( gj )( "id" ).get_int() == 7 );
  CHECK( std::as_const( gj )( "name" ).get_string() == "json" );

  auto const gy = GenericContainer::gc_from_yaml( "id: 8\nname: yaml\n" );
  CHECK( gy.get_type() == GC_type::MAP );
  CHECK( std::as_const( gy )( "id" ).get_int() == 8 );
  CHECK( std::as_const( gy )( "name" ).get_string() == "yaml" );

  auto const gt = GenericContainer::gc_from_toml( "id = 9\nname = \"toml\"\n" );
  CHECK( gt.get_type() == GC_type::MAP );
  CHECK( std::as_const( gt )( "id" ).get_int() == 9 );
  CHECK( std::as_const( gt )( "name" ).get_string() == "toml" );
}

TEST_CASE( "fixture files parse through each interface", "[interfaces]" )
{
  std::ifstream json( fixture_path( "data.json" ) );
  REQUIRE( json.good() );
  GenericContainer gj;
  CHECK( gj.from_json( json ) );
  CHECK( gj.get_type() == GC_type::MAP );

  std::ifstream yaml( fixture_path( "test1.yml" ) );
  REQUIRE( yaml.good() );
  GenericContainer gy;
  CHECK( gy.from_yaml( yaml ) );
  CHECK_FALSE( gy.empty() );

  std::ifstream toml( fixture_path( "settings.toml" ) );
  REQUIRE( toml.good() );
  GenericContainer gt;
  CHECK( gt.from_toml( toml ) );
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
