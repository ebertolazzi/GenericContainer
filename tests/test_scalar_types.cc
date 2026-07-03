/*--------------------------------------------------------------------------*\
 |  Characterization tests: scalar types (NOTYPE, POINTER, BOOL, INTEGER,   |
 |  LONG, REAL, COMPLEX, STRING).                                           |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <stdexcept>
#include <utility>

using namespace GC_namespace;
using Catch::Matchers::ContainsSubstring;

TEST_CASE( "default constructed container is empty NOTYPE", "[scalar]" )
{
  GenericContainer gc;
  CHECK( gc.empty() );
  CHECK( gc.get_type() == GC_type::NOTYPE );
  CHECK( gc.get_type_name() == "NOTYPE" );
  CHECK( gc.get_num_elements() == 0 );
}

TEST_CASE( "bool set/get round trip", "[scalar]" )
{
  GenericContainer gc;
  gc.set_bool( true );
  CHECK( gc.get_type() == GC_type::BOOL );
  CHECK( std::as_const( gc ).get_bool() == true );
  CHECK( gc.get_num_elements() == 1 );

  gc = false;
  CHECK( std::as_const( gc ).get_bool() == false );

  GenericContainer gc2{ true };
  CHECK( std::as_const( gc2 ).get_bool() == true );
}

TEST_CASE( "int set/get round trip", "[scalar]" )
{
  GenericContainer gc;
  gc.set_int( -37 );
  CHECK( gc.get_type() == GC_type::INTEGER );
  CHECK( std::as_const( gc ).get_int() == -37 );

  gc = int_type( 12 );
  CHECK( std::as_const( gc ).get_int() == 12 );
  CHECK( gc.is_number() );
  CHECK( gc.get_number() == 12.0 );
}

TEST_CASE( "long set/get round trip", "[scalar]" )
{
  GenericContainer gc;
  gc.set_long( 1LL << 40 );
  CHECK( gc.get_type() == GC_type::LONG );
  CHECK( std::as_const( gc ).get_long() == ( 1LL << 40 ) );

  gc = long_type( -5 );
  CHECK( std::as_const( gc ).get_long() == -5 );
}

TEST_CASE( "unsigned assignment maps onto signed storage", "[scalar]" )
{
  GenericContainer gc;
  gc = uint_type( 7 );
  CHECK( gc.get_type() == GC_type::INTEGER );
  CHECK( std::as_const( gc ).get_int() == 7 );

  gc = ulong_type( 9 );
  CHECK( gc.get_type() == GC_type::LONG );
  CHECK( std::as_const( gc ).get_long() == 9 );
}

TEST_CASE( "real set/get round trip", "[scalar]" )
{
  GenericContainer gc;
  gc.set_real( 2.5 );
  CHECK( gc.get_type() == GC_type::REAL );
  CHECK( std::as_const( gc ).get_real() == 2.5 );
  CHECK( gc.is_number() );
  CHECK( gc.get_number() == 2.5 );

  gc = 1.25f;  // float assignment promotes to real
  CHECK( gc.get_type() == GC_type::REAL );
  CHECK( std::as_const( gc ).get_real() == 1.25 );
}

TEST_CASE( "complex set/get round trip", "[scalar]" )
{
  GenericContainer gc;
  gc.set_complex( 1.0, -2.0 );
  CHECK( gc.get_type() == GC_type::COMPLEX );
  CHECK( std::as_const( gc ).get_complex() == complex_type( 1.0, -2.0 ) );

  real_type re{ 0 }, im{ 0 };
  gc.get_complex_number( re, im );
  CHECK( re == 1.0 );
  CHECK( im == -2.0 );

  gc = complex_type( 3.0, 4.0 );
  CHECK( std::as_const( gc ).get_complex() == complex_type( 3.0, 4.0 ) );
  CHECK( gc.get_complex_number() == complex_type( 3.0, 4.0 ) );
}

TEST_CASE( "string set/get round trip", "[scalar]" )
{
  GenericContainer gc;
  gc.set_string( "hello" );
  CHECK( gc.get_type() == GC_type::STRING );
  CHECK( std::as_const( gc ).get_string() == "hello" );

  gc = "literal";
  CHECK( std::as_const( gc ).get_string() == "literal" );

  gc = std::string( "std_string" );
  CHECK( std::as_const( gc ).get_string() == "std_string" );

  gc = string_view( "view" );
  CHECK( std::as_const( gc ).get_string() == "view" );
}

TEST_CASE( "pointer set/get round trip", "[scalar]" )
{
  int              x{ 5 };
  GenericContainer gc;
  gc.set_pointer( &x );
  CHECK( gc.get_type() == GC_type::POINTER );
  // Characterization of the asymmetric overloads: the const version returns
  // the stored pointer cast to T, the non-const version dereferences the
  // stored pointer as a T* and yields a T&.
  CHECK( std::as_const( gc ).get_pointer<int *>() == &x );
  CHECK( gc.get_pointer<int>() == 5 );
  gc.get_pointer<int>() = 6;
  CHECK( x == 6 );

  gc.free_pointer();
  CHECK( gc.get_type() == GC_type::NOTYPE );
}

TEST_CASE( "set overwrites previous content of different type", "[scalar]" )
{
  GenericContainer gc;
  gc.set_string( "will be replaced" );
  gc.set_int( 3 );
  CHECK( gc.get_type() == GC_type::INTEGER );
  CHECK( std::as_const( gc ).get_int() == 3 );

  gc.set_vec_real( 4 );
  gc.set_bool( true );
  CHECK( gc.get_type() == GC_type::BOOL );
}

TEST_CASE( "clear resets to NOTYPE", "[scalar]" )
{
  GenericContainer gc;
  gc.set_string( "content" );
  gc.clear();
  CHECK( gc.empty() );
  CHECK( gc.get_type() == GC_type::NOTYPE );
}

TEST_CASE( "const get with wrong type throws with type names in message", "[scalar][errors]" )
{
  GenericContainer gc;
  gc.set_int( 1 );
  GenericContainer const & cgc = gc;
  CHECK_THROWS_AS( cgc.get_string(), std::runtime_error );
  CHECK_THROWS_AS( cgc.get_bool(), std::runtime_error );
  CHECK_THROWS_AS( cgc.get_complex(), std::runtime_error );
  // to_string(GC_type) spells types as "string_type", "int_type", ...
  CHECK_THROWS_WITH( cgc.get_string(), ContainsSubstring( "string_type" ) && ContainsSubstring( "int_type" ) );
}

TEST_CASE( "get_number converts numeric scalars, throws otherwise", "[scalar]" )
{
  GenericContainer gc;
  gc.set_bool( true );
  CHECK( gc.get_number() == 1.0 );
  gc.set_int( 5 );
  CHECK( gc.get_number() == 5.0 );
  gc.set_long( 7 );
  CHECK( gc.get_number() == 7.0 );
  gc.set_real( 0.5 );
  CHECK( gc.get_number() == 0.5 );

  gc.set_string( "no" );
  CHECK_THROWS_AS( gc.get_number(), std::runtime_error );
}

TEST_CASE( "get_as_int / get_as_long convert in-range values", "[scalar]" )
{
  GenericContainer gc;
  gc.set_bool( true );
  CHECK( gc.get_as_int() == 1 );
  gc.set_int( -4 );
  CHECK( gc.get_as_int() == -4 );
  CHECK( gc.get_as_long() == -4 );
  gc.set_long( 123 );
  CHECK( gc.get_as_int() == 123 );
  gc.set_real( 8.0 );
  CHECK( gc.get_as_int() == 8 );
  CHECK( gc.get_as_uint() == 8u );
  CHECK( gc.get_as_ulong() == 8u );
}

TEST_CASE( "get_value copies into out-parameter", "[scalar]" )
{
  GenericContainer gc;
  gc.set_real( 6.0 );
  int_type i{ 0 };
  gc.get_value( i );
  CHECK( i == 6 );

  real_type r{ 0 };
  gc.get_value( r );
  CHECK( r == 6.0 );
}

TEST_CASE( "non-const scalar getter on NOTYPE defines the type", "[scalar][autoset]" )
{
  // Characterization: mutable get_* on an empty container allocates the type
  // (ck_or_set) instead of throwing.
  GenericContainer gc;
  gc.get_int() = 9;
  CHECK( gc.get_type() == GC_type::INTEGER );
  CHECK( std::as_const( gc ).get_int() == 9 );

  GenericContainer gs;
  gs.get_string() = "written";
  CHECK( gs.get_type() == GC_type::STRING );
  CHECK( std::as_const( gs ).get_string() == "written" );
}
