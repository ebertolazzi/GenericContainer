/*--------------------------------------------------------------------------*\
 |  Characterization tests: deep copy, move semantics, swap, aliasing.      |
 |  Self-assignment/aliasing cases are [newbehavior]: the current copy      |
 |  assignment is clear(); from_gc(a); which is a use-after-free for them.  |
 |  They are skipped until the Phase 2 storage rewrite fixes assignment.    |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>

#include <utility>
#include <vector>

using namespace GC_namespace;
using gc_test::gc_equal;

namespace
{
  //! One maker per stored type, for exhaustive copy/move coverage.
  std::vector<GenericContainer> make_one_of_each()
  {
    static int                    x{ 1 };
    std::vector<GenericContainer> out;
    GenericContainer              gc;

    out.emplace_back();  // NOTYPE
    gc.set_pointer( &x );
    out.push_back( gc );
    gc.set_bool( true );
    out.push_back( gc );
    gc.set_int( 3 );
    out.push_back( gc );
    gc.set_long( 1LL << 40 );
    out.push_back( gc );
    gc.set_real( 2.5 );
    out.push_back( gc );
    gc.set_complex( 1.0, -1.0 );
    out.push_back( gc );
    gc.set_string( "deep" );
    out.push_back( gc );
    gc.set_vec_pointer( vec_pointer_type{ &x } );
    out.push_back( gc );
    gc.set_vec_bool( vec_bool_type{ true, false } );
    out.push_back( gc );
    gc.set_vec_int( vec_int_type{ 1, 2 } );
    out.push_back( gc );
    gc.set_vec_long( vec_long_type{ 1LL << 33 } );
    out.push_back( gc );
    gc.set_vec_real( vec_real_type{ 0.5 } );
    out.push_back( gc );
    gc.set_vec_complex( vec_complex_type{ complex_type( 1, 2 ) } );
    out.push_back( gc );
    gc.set_vec_string( vec_string_type{ "a", "b" } );
    out.push_back( gc );

    auto & mi   = gc.set_mat_int( 2, 2 );
    mi( 1, 0 )  = 5;
    out.push_back( gc );
    auto & ml   = gc.set_mat_long( 1, 2 );
    ml( 0, 1 )  = 1LL << 36;
    out.push_back( gc );
    auto & mr   = gc.set_mat_real( 2, 1 );
    mr( 1, 0 )  = 0.25;
    out.push_back( gc );
    auto & mc   = gc.set_mat_complex( 1, 1 );
    mc( 0, 0 )  = complex_type( 3, -3 );
    out.push_back( gc );

    auto & v = gc.set_vector( 2 );
    v[0]     = 1;
    v[1]     = "two";
    out.push_back( gc );

    gc.clear();
    gc["k"]["nested"] = vec_real_type{ 1.5 };
    out.push_back( gc );

    return out;
  }
}  // namespace

TEST_CASE( "copy constructor deep-copies every stored type", "[copy]" )
{
  for ( auto const & original : make_one_of_each() )
  {
    GenericContainer copy( original );
    CHECK( copy.get_type() == original.get_type() );
    CHECK( gc_equal( copy, original ) );
  }
}

TEST_CASE( "copy assignment deep-copies every stored type", "[copy]" )
{
  for ( auto const & original : make_one_of_each() )
  {
    GenericContainer copy;
    copy = original;
    CHECK( gc_equal( copy, original ) );

    GenericContainer overwrite;
    overwrite.set_string( "previous content" );
    overwrite = original;
    CHECK( gc_equal( overwrite, original ) );
  }
}

TEST_CASE( "copies are independent of the original", "[copy]" )
{
  GenericContainer original;
  original["values"] = vec_int_type{ 1, 2, 3 };
  original["name"]   = "orig";

  GenericContainer copy( original );
  copy["values"].get_vec_int()[0] = 99;
  copy["name"]                    = "copy";
  copy["extra"]                   = true;

  CHECK( std::as_const( original )["values"].get_vec_int() == vec_int_type{ 1, 2, 3 } );
  CHECK( std::as_const( original )["name"].get_string() == "orig" );
  CHECK_FALSE( original.exists( "extra" ) );
}

TEST_CASE( "load() replaces content with a deep copy", "[copy]" )
{
  GenericContainer src;
  src = vec_real_type{ 1.0, 2.0 };

  GenericContainer dst;
  dst.set_string( "old" );
  dst.load( src );
  CHECK( gc_equal( dst, src ) );
}

TEST_CASE( "to_gc / from_gc deep copy", "[copy]" )
{
  GenericContainer src;
  src["a"] = 1;
  src["b"] = vec_string_type{ "x" };

  GenericContainer via_to;
  src.to_gc( via_to );
  CHECK( gc_equal( via_to, src ) );

  GenericContainer via_from;
  via_from.from_gc( src );
  CHECK( gc_equal( via_from, src ) );
}

TEST_CASE( "move constructor transfers content and empties source", "[move]" )
{
  for ( auto & original : make_one_of_each() )
  {
    GenericContainer reference( original );
    GenericContainer moved( std::move( original ) );
    CHECK( gc_equal( moved, reference ) );
    CHECK( original.get_type() == GC_type::NOTYPE );
    CHECK( original.empty() );
  }
}

TEST_CASE( "move assignment transfers content and empties source", "[move]" )
{
  for ( auto & original : make_one_of_each() )
  {
    GenericContainer reference( original );
    GenericContainer target;
    target.set_string( "to be replaced" );
    target = std::move( original );
    CHECK( gc_equal( target, reference ) );
    CHECK( original.empty() );
  }
}

TEST_CASE( "moved-from container is reusable", "[move]" )
{
  GenericContainer a;
  a = vec_int_type{ 1, 2 };
  GenericContainer b( std::move( a ) );
  a.set_real( 4.5 );
  CHECK( std::as_const( a ).get_real() == 4.5 );
  CHECK( std::as_const( b ).get_vec_int() == vec_int_type{ 1, 2 } );
}

TEST_CASE( "swap exchanges contents", "[move]" )
{
  GenericContainer a, b;
  a = 1;
  b = "two";
  a.swap( b );
  CHECK( std::as_const( a ).get_string() == "two" );
  CHECK( std::as_const( b ).get_int() == 1 );
}

TEST_CASE( "self move-assignment is harmless", "[move]" )
{
  GenericContainer a;
  a = vec_real_type{ 1.5 };
  GenericContainer & alias = a;
  a = std::move( alias );
  // Standard containers only guarantee "valid state"; current implementation
  // keeps the content (self-move is a no-op via the self check).
  CHECK( std::as_const( a ).get_vec_real() == vec_real_type{ 1.5 } );
}

TEST_CASE( "self copy-assignment keeps content", "[copy][newbehavior]" )
{
  GenericContainer a;
  a["k"] = vec_int_type{ 1, 2 };
  GenericContainer & alias = a;
  a = alias;
  CHECK( std::as_const( a )["k"].get_vec_int() == vec_int_type{ 1, 2 } );
}

TEST_CASE( "assignment from own sub-element keeps the sub-tree alive", "[copy][newbehavior]" )
{
  GenericContainer a;
  a["sub"]["x"] = 42;
  a = std::as_const( a )["sub"];
  CHECK( std::as_const( a )["x"].get_int() == 42 );
}

TEST_CASE( "set_vec_int from own vector reference survives", "[copy][newbehavior]" )
{
  GenericContainer a;
  a = vec_int_type{ 3, 4 };
  a.set_vec_int( std::as_const( a ).get_vec_int() );
  CHECK( std::as_const( a ).get_vec_int() == vec_int_type{ 3, 4 } );
}
