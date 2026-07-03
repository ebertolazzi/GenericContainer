/*--------------------------------------------------------------------------*\
 |  Characterization tests: from_file dispatch and misc support behavior.   |
 |  Fixtures live under examples/ (tests run from the repo root).           |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>

using namespace GC_namespace;

TEST_CASE( "from_file dispatches on extension", "[support]" )
{
  GenericContainer gc;
  CHECK( gc.from_file( "examples/test.json" ) );
  CHECK_FALSE( gc.empty() );

  GenericContainer gy;
  CHECK( gy.from_file( "examples/test.yml" ) );
  CHECK_FALSE( gy.empty() );

  GenericContainer gt;
  CHECK( gt.from_file( "examples/test.toml" ) );
  CHECK_FALSE( gt.empty() );
}

TEST_CASE( "from_file returns false for missing file or unknown extension", "[support]" )
{
  GenericContainer gc;
  CHECK_FALSE( gc.from_file( "examples/does_not_exist.json" ) );
  CHECK_FALSE( gc.from_file( "examples/example07_data.txt" ) );  // no handler for .txt
}
