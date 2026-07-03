/*--------------------------------------------------------------------------*\
 |  Characterization tests: formatted-data tables (write/read round trips). |
 |  Fixture: examples/example07_data.txt (tests run from the repo root).    |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include <stdexcept>
#include <utility>

using namespace GC_namespace;

TEST_CASE( "write_formatted_data requires headers and data fields", "[tables]" )
{
  GenericContainer   gc;
  std::ostringstream out;
  CHECK_THROWS_AS( gc.write_formatted_data( out ), std::runtime_error );

  gc["headers"] = vec_string_type{ "a", "b" };
  CHECK_THROWS_AS( gc.write_formatted_data( out ), std::runtime_error );
}

TEST_CASE( "write / read formatted data round trip via stream", "[tables]" )
{
  GenericContainer gc;
  gc["headers"] = vec_string_type{ "x", "y" };
  auto & data   = gc["data"].set_vector( 2 );
  data[0]       = vec_real_type{ 1.0, 2.0, 3.0 };
  data[1]       = vec_real_type{ 0.5, 1.5, 2.5 };

  std::ostringstream out;
  gc.write_formatted_data( out );
  CHECK_FALSE( out.str().empty() );

  std::istringstream in( out.str() );
  GenericContainer   back;
  back.read_formatted_data( in );
  CHECK( back.get_type() == GC_type::MAP );
  CHECK( back.exists( "headers" ) );
  CHECK( back.exists( "data" ) );
  CHECK( std::as_const( back )( "headers" ).get_vec_string() == vec_string_type{ "x", "y" } );

  auto const & rdata = std::as_const( back )( "data" );
  CHECK( rdata.get_num_elements() == 2 );
  CHECK( rdata( 0 ).get_vec_real() == vec_real_type{ 1.0, 2.0, 3.0 } );
  CHECK( rdata( 1 ).get_vec_real() == vec_real_type{ 0.5, 1.5, 2.5 } );
}

TEST_CASE( "read_formatted_data parses the example fixture file", "[tables]" )
{
  GenericContainer gc;
  gc.read_formatted_data( "examples/example07_data.txt" );
  CHECK( gc.get_type() == GC_type::MAP );
  CHECK( gc.exists( "headers" ) );
  CHECK( gc.exists( "data" ) );
  CHECK( std::as_const( gc )( "headers" ).get_vec_string().size() > 0 );

  auto const & data = std::as_const( gc )( "data" );
  CHECK( data.get_num_elements() == std::as_const( gc )( "headers" ).get_vec_string().size() );

  GenericContainer missing;
  CHECK_THROWS_AS( missing.read_formatted_data( "examples/no_such_file.txt" ), std::runtime_error );
}

TEST_CASE( "read_formatted_data2 parses fixture with column vectors", "[tables]" )
{
  GenericContainer gc;
  gc.read_formatted_data2( "examples/example07_data.txt" );
  CHECK( gc.get_type() == GC_type::MAP );
}
