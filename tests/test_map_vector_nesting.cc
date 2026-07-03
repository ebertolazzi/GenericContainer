/*--------------------------------------------------------------------------*\
 |  Characterization tests: maps, vectors of GC, nesting, map queries.      |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <stdexcept>
#include <utility>

using namespace GC_namespace;
using Catch::Matchers::ContainsSubstring;

TEST_CASE( "map builder pattern with operator[]", "[map]" )
{
  GenericContainer gc;
  gc["alpha"] = 1;
  gc["beta"]  = "two";
  gc["gamma"] = 3.5;

  CHECK( gc.get_type() == GC_type::MAP );
  CHECK( gc.get_num_elements() == 3 );
  CHECK( std::as_const( gc )["alpha"].get_int() == 1 );
  CHECK( std::as_const( gc )["beta"].get_string() == "two" );
  CHECK( std::as_const( gc )["gamma"].get_real() == 3.5 );
}

TEST_CASE( "deep nesting vector-of-map-of-vector", "[map][nesting]" )
{
  GenericContainer gc;
  auto &           v = gc.set_vector( 2 );
  v[0]["name"]       = "first";
  v[0]["values"]     = vec_int_type{ 1, 2, 3 };
  v[1]["name"]       = "second";
  v[1]["child"][0]   = true;
  v[1]["child"][1]   = vec_real_type{ 0.5 };

  GenericContainer const & c = gc;
  CHECK( c( 0 )( "name" ).get_string() == "first" );
  CHECK( c( 0 )( "values" ).get_vec_int() == vec_int_type{ 1, 2, 3 } );
  CHECK( c( 1 )( "child" )( 0 ).get_bool() == true );
  CHECK( c( 1 )( "child" )( 1 ).get_vec_real() == vec_real_type{ 0.5 } );
}

TEST_CASE( "get_keys lists map keys", "[map]" )
{
  GenericContainer gc;
  gc["k1"] = 1;
  gc["k2"] = 2;

  vec_string_type keys;
  gc.get_keys( keys );
  CHECK( keys == vec_string_type{ "k1", "k2" } );
  CHECK_THAT( gc.get_keys(), ContainsSubstring( "k1" ) && ContainsSubstring( "k2" ) );
}

TEST_CASE( "exists and must_exists", "[map]" )
{
  GenericContainer gc;
  gc["present"] = 1;

  CHECK( gc.exists( "present" ) );
  CHECK_FALSE( gc.exists( "absent" ) );
  CHECK( gc.exists( vec_string_type{ "nope", "present" } ) );
  CHECK_FALSE( gc.exists( vec_string_type{ "nope", "still_nope" } ) );

  CHECK( gc.must_exists( vec_string_type{ "present" }, "test" ) == "present" );
  CHECK_THROWS_AS( gc.must_exists( vec_string_type{ "absent" }, "test" ), std::runtime_error );
}

TEST_CASE( "get_if_exists retrieves typed values", "[map]" )
{
  GenericContainer gc;
  gc["b"] = true;
  gc["i"] = 42;
  gc["r"] = 2.5;
  gc["s"] = "str";
  gc["c"] = complex_type( 1, 2 );
  gc["l"] = long_type( 1LL << 35 );

  bool bv{ false };
  CHECK( gc.get_if_exists( "b", bv ) );
  CHECK( bv == true );

  int_type iv{ 0 };
  CHECK( gc.get_if_exists( "i", iv ) );
  CHECK( iv == 42 );

  real_type rv{ 0 };
  CHECK( gc.get_if_exists( "r", rv ) );
  CHECK( rv == 2.5 );

  string_type sv;
  CHECK( gc.get_if_exists( "s", sv ) );
  CHECK( sv == "str" );

  complex_type cv;
  CHECK( gc.get_if_exists( "c", cv ) );
  CHECK( cv == complex_type( 1, 2 ) );

  long_type lv{ 0 };
  CHECK( gc.get_if_exists( "l", lv ) );
  CHECK( lv == ( 1LL << 35 ) );

  int_type missing{ -1 };
  CHECK_FALSE( gc.get_if_exists( "nope", missing ) );
  CHECK( missing == -1 );

  // vector-of-fields overload: first matching key wins
  int_type multi{ 0 };
  CHECK( gc.get_if_exists( vec_string_type{ "zz", "i" }, multi ) );
  CHECK( multi == 42 );
}

TEST_CASE( "get_map_* typed accessors with key", "[map]" )
{
  GenericContainer gc;
  gc["flag"]  = true;
  gc["count"] = 3;
  gc["ratio"] = 0.75;
  gc["name"]  = "gc";
  gc["vr"]    = vec_real_type{ 1.0, 2.0 };
  gc["vs"]    = vec_string_type{ "a" };
  gc["vc"]    = vec_complex_type{ complex_type( 1, 1 ) };

  CHECK( gc.get_map_bool( "flag" ) == true );
  CHECK( gc.get_map_int( "count" ) == 3 );
  CHECK( gc.get_map_number( "ratio" ) == 0.75 );
  CHECK( gc.get_map_string( "name" ) == "gc" );
  CHECK( gc.get_map_vec_real( "vr" ) == vec_real_type{ 1.0, 2.0 } );
  CHECK( gc.get_map_vec_string( "vs" ) == vec_string_type{ "a" } );
  CHECK( gc.get_map_vec_complex( "vc" ) == vec_complex_type{ complex_type( 1, 1 ) } );

  // number accessor converts int
  CHECK( gc.get_map_number( "count" ) == 3.0 );
}

TEST_CASE( "get_map_* initializer-list overload searches keys", "[map]" )
{
  GenericContainer gc;
  gc["flag"] = true;
  gc["n"]    = 5;

  CHECK( gc.get_map_bool( { "missing", "flag" } ) == true );
  CHECK( gc.get_map_int( { "n" } ) == 5 );
  CHECK_THROWS_AS( gc.get_map_bool( { "a", "b" } ), std::runtime_error );
}

TEST_CASE( "erase removes a key", "[map]" )
{
  GenericContainer gc;
  gc["keep"]   = 1;
  gc["remove"] = 2;
  gc.erase( "remove" );
  CHECK( gc.get_num_elements() == 1 );
  CHECK_FALSE( gc.exists( "remove" ) );

  GenericContainer notmap;
  notmap = 5;
  CHECK_THROWS_AS( notmap.erase( "x" ), std::runtime_error );
}

TEST_CASE( "merge combines maps recursively", "[map]" )
{
  GenericContainer a;
  a["x"] = 1;
  a["y"] = 2;

  GenericContainer b;
  b["y"] = 20;
  b["z"] = 30;

  a.merge( b, "test" );
  CHECK( a.get_num_elements() == 3 );
  CHECK( std::as_const( a )["x"].get_int() == 1 );
  CHECK( std::as_const( a )["y"].get_int() == 20 );  // b wins on conflicts
  CHECK( std::as_const( a )["z"].get_int() == 30 );

  // merging NOTYPE is a no-op; merging into NOTYPE creates the map
  GenericContainer empty;
  a.merge( empty, "test" );
  CHECK( a.get_num_elements() == 3 );

  GenericContainer target;
  target.merge( b, "test" );
  CHECK( target.get_type() == GC_type::MAP );
  CHECK( target.get_num_elements() == 2 );

  // merging a non-map throws
  GenericContainer scalar;
  scalar = 5;
  CHECK_THROWS_AS( a.merge( scalar, "test" ), std::runtime_error );
}

TEST_CASE( "get_map returns underlying map", "[map]" )
{
  GenericContainer gc;
  gc["a"] = 1;
  auto & m = gc.get_map();
  CHECK( m.size() == 1 );
  CHECK( std::as_const( gc ).get_map().count( "a" ) == 1 );

  GenericContainer notmap;
  notmap = 2.0;
  CHECK_THROWS_AS( std::as_const( notmap ).get_map(), std::runtime_error );
}

TEST_CASE( "deeply nested vector chain constructs and destructs", "[nesting][recursion]" )
{
  // clear()/destructor recurse through nested containers; pin that a
  // reasonably deep chain (2000 levels) works.
  constexpr int      depth = 2000;
  GenericContainer   root;
  GenericContainer * cur = &root;
  for ( int k = 0; k < depth; ++k )
  {
    auto & v = cur->set_vector( 1 );
    cur      = &v[0];
  }
  cur->set_int( 7 );

  GenericContainer const * walk = &root;
  for ( int k = 0; k < depth; ++k ) walk = &( *walk )( 0 );
  CHECK( walk->get_int() == 7 );

  root.clear();  // must not overflow the stack
  CHECK( root.empty() );
}
