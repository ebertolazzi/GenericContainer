/*--------------------------------------------------------------------------*\
 |  Smoke tests: info/dump/print/print_content_types on a maximal          |
 |  container. Structure-level checks only, not byte-exact text.            |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <sstream>

using namespace GC_namespace;
using Catch::Matchers::ContainsSubstring;
using gc_test::make_maximal_gc;

TEST_CASE( "dump renders every type without throwing", "[print]" )
{
  GenericContainer const gc{ make_maximal_gc() };
  std::ostringstream     out;
  CHECK_NOTHROW( gc.dump( out ) );
  CHECK_FALSE( out.str().empty() );
  CHECK_THAT( out.str(), ContainsSubstring( "hello" ) && ContainsSubstring( "nested" ) );
}

TEST_CASE( "print is dump plus trailing content", "[print]" )
{
  GenericContainer const gc{ make_maximal_gc() };
  std::ostringstream     out;
  CHECK_NOTHROW( gc.print( out ) );
  CHECK_FALSE( gc.print().empty() );
}

TEST_CASE( "info summarizes type", "[print]" )
{
  GenericContainer gc;
  gc = vec_real_type{ 1.0, 2.0 };
  CHECK_FALSE( gc.info().empty() );

  std::ostringstream out;
  gc.info( out );
  CHECK_FALSE( out.str().empty() );
}

TEST_CASE( "print_content_types walks the tree", "[print]" )
{
  GenericContainer const gc{ make_maximal_gc() };
  std::ostringstream     out;
  CHECK_NOTHROW( gc.print_content_types( out ) );
  CHECK_FALSE( out.str().empty() );
}

TEST_CASE( "empty container prints without throwing", "[print]" )
{
  GenericContainer   gc;
  std::ostringstream out;
  CHECK_NOTHROW( gc.dump( out ) );
  CHECK_NOTHROW( gc.print_content_types( out ) );
}
