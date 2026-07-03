/*--------------------------------------------------------------------------*\
 |  Characterization tests: operator[] / operator() semantics.              |
 |                                                                          |
 |  KEPT BY DESIGN: mutating operator[] silently destroys content of a      |
 |  different type (auto-convert to VECTOR/MAP) and auto-resizes vectors.   |
 |  These tests pin that behavior; operator() is the checked accessor.      |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <stdexcept>
#include <utility>

using namespace GC_namespace;
using Catch::Matchers::ContainsSubstring;

TEST_CASE( "mutating operator[](i) on NOTYPE creates a vector", "[operators]" )
{
  GenericContainer gc;
  gc[2] = 5;
  CHECK( gc.get_type() == GC_type::VECTOR );
  CHECK( gc.get_num_elements() == 3 );
  CHECK( std::as_const( gc )( 2 ).get_int() == 5 );
  CHECK( std::as_const( gc )( 0 ).empty() );
}

TEST_CASE( "mutating operator[](i) destroys non-vector content", "[operators][autoconvert]" )
{
  GenericContainer gc;
  gc.set_int( 42 );
  gc[0] = "replaced";
  CHECK( gc.get_type() == GC_type::VECTOR );
  CHECK( std::as_const( gc )( 0 ).get_string() == "replaced" );
}

TEST_CASE( "mutating operator[](i) auto-resizes the vector", "[operators][autoconvert]" )
{
  GenericContainer gc;
  gc.set_vector( 1 );
  gc[4] = 1.5;
  CHECK( gc.get_num_elements() == 5 );
  CHECK( std::as_const( gc )( 4 ).get_real() == 1.5 );
}

TEST_CASE( "mutating operator[](key) on NOTYPE creates a map", "[operators]" )
{
  GenericContainer gc;
  gc["k"] = 1;
  CHECK( gc.get_type() == GC_type::MAP );
  CHECK( std::as_const( gc )( "k" ).get_int() == 1 );
}

TEST_CASE( "mutating operator[](key) destroys non-map content", "[operators][autoconvert]" )
{
  GenericContainer gc;
  gc = vec_int_type{ 1, 2, 3 };
  gc["k"] = true;
  CHECK( gc.get_type() == GC_type::MAP );
  CHECK( gc.get_num_elements() == 1 );
  CHECK( std::as_const( gc )( "k" ).get_bool() == true );
}

TEST_CASE( "const operator[](i) checks type and range", "[operators][errors]" )
{
  GenericContainer gc;
  gc.set_vector( 2 );
  GenericContainer const & cgc = gc;
  CHECK_NOTHROW( cgc[1] );
  CHECK_THROWS_AS( cgc[5], std::runtime_error );

  GenericContainer         scalar;
  scalar = 3;
  GenericContainer const & cscalar = scalar;
  CHECK_THROWS_AS( cscalar[0], std::runtime_error );
}

TEST_CASE( "const operator[](key) throws on missing key listing candidates", "[operators][errors]" )
{
  GenericContainer gc;
  gc["exists"] = 1;
  GenericContainer const & cgc = gc;
  CHECK_NOTHROW( cgc["exists"] );
  CHECK_THROWS_WITH( cgc["missing"], ContainsSubstring( "missing" ) && ContainsSubstring( "exists" ) );

  GenericContainer         scalar;
  scalar = 3;
  GenericContainer const & cscalar = scalar;
  CHECK_THROWS_AS( cscalar["k"], std::runtime_error );
}

TEST_CASE( "operator() never mutates: wrong type or index throws", "[operators][errors]" )
{
  GenericContainer gc;  // NOTYPE
  CHECK_THROWS_AS( gc( 0 ), std::runtime_error );
  CHECK_THROWS_AS( gc( "k" ), std::runtime_error );

  gc.set_vector( 2 );
  CHECK_NOTHROW( gc( 1 ) );
  CHECK_THROWS_AS( gc( 2 ), std::runtime_error );

  GenericContainer m;
  m["a"] = 1;
  CHECK_NOTHROW( m( "a" ) );
  CHECK_THROWS_AS( m( "b" ), std::runtime_error );
}

TEST_CASE( "operator() propagates the where context into error messages", "[operators][errors]" )
{
  GenericContainer gc;
  gc.set_vector( 1 );
  CHECK_THROWS_WITH( gc( 9, "CALLSITE_MARKER" ), ContainsSubstring( "CALLSITE_MARKER" ) );

  GenericContainer m;
  m["a"] = 1;
  CHECK_THROWS_WITH( m( "nope", "OTHER_MARKER" ), ContainsSubstring( "OTHER_MARKER" ) );
}

TEST_CASE( "operator() with key-candidate vector returns first match", "[operators]" )
{
  GenericContainer gc;
  gc["second"] = 2;
  CHECK( gc( vec_string_type{ "first", "second" } ).get_int() == 2 );
  CHECK( std::as_const( gc )( vec_string_type{ "second" } ).get_int() == 2 );
  CHECK_THROWS_AS( gc( vec_string_type{ "no", "nope" } ), std::runtime_error );
}

TEST_CASE( "get_gc_at accesses vector elements", "[operators]" )
{
  GenericContainer gc;
  auto &           v = gc.set_vector( 2 );
  v[1]               = "elem";
  CHECK( gc.get_gc_at( 1 ).get_string() == "elem" );
  CHECK( std::as_const( gc ).get_gc_at( 1, "test" ).get_string() == "elem" );
  CHECK_THROWS_AS( std::as_const( gc ).get_gc_at( 7, "test" ), std::runtime_error );
}
