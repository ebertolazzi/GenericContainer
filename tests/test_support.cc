/*--------------------------------------------------------------------------*\
 |  Characterization tests: from_file dispatch and misc support behavior.   |
 |  Fixtures live under examples/.                                          |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>

using namespace GC_namespace;

namespace
{
  static std::filesystem::path
  fixture_path( char const * name )
  {
    return std::filesystem::path( __FILE__ ).parent_path().parent_path() / "examples" / name;
  }
}

TEST_CASE( "from_file dispatches on extension", "[support]" )
{
  GenericContainer gc;
  CHECK_FALSE( gc.from_file( fixture_path( "test.json" ).string() ) );
  CHECK( gc.empty() );

  GenericContainer gy;
  CHECK_THROWS_AS( gy.from_file( fixture_path( "test.yml" ).string() ), std::runtime_error );
  CHECK( gy.empty() );

  GenericContainer gt;
  CHECK_THROWS_AS( gt.from_file( fixture_path( "test.toml" ).string() ), std::runtime_error );
  CHECK( gt.empty() );
}

TEST_CASE( "from_file returns false for missing file or unknown extension", "[support]" )
{
  GenericContainer gc;
  CHECK_FALSE( gc.from_file( fixture_path( "does_not_exist.json" ).string() ) );
  CHECK_FALSE( gc.from_file( fixture_path( "example07_data.txt" ).string() ) );  // no handler for .txt
}
