/*--------------------------------------------------------------------------*\
 |  Characterization tests: nlohmann::json <-> GenericContainer via the     |
 |  adl_serializer specialization.                                          |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <nlohmann/json.hpp>

#include "GenericContainer/GenericContainerInterface_nlohmann.hh"

#include <catch2/catch_test_macros.hpp>

#include <fstream>

using namespace GC_namespace;
using nlohmann::json;

TEST_CASE( "GC to json for scalars", "[nlohmann]" )
{
  GenericContainer gc;
  gc["b"] = true;
  gc["i"] = 42;
  gc["r"] = 2.5;
  gc["s"] = "text";

  json const j = gc;
  CHECK( j["b"] == true );
  CHECK( j["i"] == 42 );
  CHECK( j["r"] == 2.5 );
  CHECK( j["s"] == "text" );
}

TEST_CASE( "json to GC and back preserves structure", "[nlohmann]" )
{
  json const j = json::parse( R"({
    "flag": false,
    "count": 3,
    "values": [1.5, 2.5],
    "names": ["a", "b"],
    "nested": {"deep": [1, 2, 3]}
  })" );

  auto const gc = j.get<GenericContainer>();
  CHECK( gc.get_type() == GC_type::MAP );
  CHECK( gc( "flag" ).get_bool() == false );
  CHECK( gc( "nested" )( "deep" ).get_type() == GC_type::VEC_INTEGER );

  json const back = gc;
  CHECK( back == j );
}

TEST_CASE( "typed vectors map onto json arrays", "[nlohmann]" )
{
  GenericContainer gc;
  gc = vec_int_type{ 1, 2, 3 };
  json j = gc;
  CHECK( j.is_array() );
  CHECK( j == json( { 1, 2, 3 } ) );

  gc = vec_string_type{ "x", "y" };
  j = gc;
  CHECK( j == json( { "x", "y" } ) );
}

TEST_CASE( "matrix maps onto array of columns", "[nlohmann]" )
{
  GenericContainer gc;
  auto &           m = gc.set_mat_real( 2, 3 );
  for ( unsigned j{ 0 }; j < 3; ++j )
    for ( unsigned i{ 0 }; i < 2; ++i ) m( i, j ) = 10.0 * i + j;

  json const j = gc;
  REQUIRE( j.is_array() );
  // pin the orientation used by the serializer
  CHECK( j.size() == 3 );          // one entry per column
  CHECK( j[0] == json( { 0.0, 10.0 } ) );
  CHECK( j[2] == json( { 2.0, 12.0 } ) );
}

TEST_CASE( "data.json fixture round trips structurally", "[nlohmann]" )
{
  std::ifstream in( "examples/data.json" );
  REQUIRE( in.good() );
  json const j = json::parse( in );

  auto const gc   = j.get<GenericContainer>();
  json const back = gc;
  CHECK( back == j );
}
