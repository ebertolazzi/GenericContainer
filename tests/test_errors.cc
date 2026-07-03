/*--------------------------------------------------------------------------*\
 |  Characterization tests: exception machinery and message content.        |
 |  Exceptions are std::runtime_error today; Phase 4 introduces             |
 |  GenericError derived from it, so these tests stay valid.                |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <stdexcept>

using namespace GC_namespace;
using Catch::Matchers::ContainsSubstring;

TEST_CASE( "exception() static helper throws runtime_error", "[errors]" )
{
  CHECK_THROWS_AS( GenericContainer::exception( "boom" ), std::runtime_error );
  CHECK_THROWS_WITH( GenericContainer::exception( "boom" ), ContainsSubstring( "boom" ) );
}

TEST_CASE( "type mismatch message names expected and actual types", "[errors]" )
{
  GenericContainer gc;
  gc.set_real( 1.0 );
  GenericContainer const & cgc = gc;
  CHECK_THROWS_WITH(
    cgc.get_vec_int(), ContainsSubstring( "vec_int_type" ) && ContainsSubstring( "real_type" ) );
}

TEST_CASE( "where context is prepended to accessor errors", "[errors]" )
{
  GenericContainer gc;
  gc.set_int( 1 );
  GenericContainer const & cgc = gc;
  CHECK_THROWS_WITH( cgc.get_string( "MY_CONTEXT" ), ContainsSubstring( "MY_CONTEXT" ) );
  CHECK_THROWS_WITH( cgc.get_vec_real( "OTHER_CTX" ), ContainsSubstring( "OTHER_CTX" ) );
}

TEST_CASE( "missing map key error lists available keys", "[errors]" )
{
  GenericContainer gc;
  gc["alpha"] = 1;
  gc["beta"]  = 2;
  GenericContainer const & cgc = gc;
  CHECK_THROWS_WITH(
    cgc( "gamma" ),
    ContainsSubstring( "gamma" ) && ContainsSubstring( "alpha" ) && ContainsSubstring( "beta" ) );
}

TEST_CASE( "all container errors derive from std::exception", "[errors]" )
{
  GenericContainer gc;
  gc.set_int( 1 );
  try
  {
    std::as_const( gc ).get_string();
    FAIL( "expected a throw" );
  }
  catch ( std::exception const & e )
  {
    CHECK( e.what() != nullptr );
  }
}
