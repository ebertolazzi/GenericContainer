/*--------------------------------------------------------------------------*\
 |  Characterization tests: binary serialization round trips, wire-format   |
 |  golden pin, and corrupted/truncated buffer handling.                    |
\*--------------------------------------------------------------------------*/

#include "gc_test_utils.hh"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <random>
#include <stdexcept>
#include <string>
#include <span>
#include <vector>

using namespace GC_namespace;
using gc_test::gc_equal;
using gc_test::make_maximal_gc;
using gc_test::make_random_gc;

namespace
{
  GenericContainer round_trip( GenericContainer const & src )
  {
    std::vector<uint8_t> buffer{ src.serialize() };
    GenericContainer     dst;
    dst.de_serialize( buffer );
    return dst;
  }
}  // namespace

TEST_CASE( "serialize round trips every scalar type", "[serialize]" )
{
  GenericContainer gc;

  CHECK( round_trip( gc ).empty() );  // NOTYPE

  gc.set_bool( true );
  CHECK( gc_equal( round_trip( gc ), gc ) );

  gc.set_int( -42 );
  CHECK( gc_equal( round_trip( gc ), gc ) );

  gc.set_long( 1LL << 40 );
  CHECK( gc_equal( round_trip( gc ), gc ) );

  gc.set_real( 3.5 );
  CHECK( gc_equal( round_trip( gc ), gc ) );

  gc.set_complex( 1.0, -2.0 );
  CHECK( gc_equal( round_trip( gc ), gc ) );

  gc.set_string( "serialize me" );
  CHECK( gc_equal( round_trip( gc ), gc ) );
}

TEST_CASE( "serialize round trips vectors, matrices, nested structures", "[serialize]" )
{
  GenericContainer gc{ make_maximal_gc() };
  GenericContainer back{ round_trip( gc ) };
  CHECK( gc_equal( back, gc ) );
}

TEST_CASE( "mem_size matches serialized size", "[serialize]" )
{
  GenericContainer gc{ make_maximal_gc() };
  std::vector<uint8_t> buffer{ gc.serialize() };
  CHECK( gc.mem_size() == int32_t( buffer.size() ) );

  GenericContainer empty;
  CHECK( empty.mem_size() == int32_t( empty.serialize().size() ) );
}

TEST_CASE( "serialize into caller buffer with exact and oversized capacity", "[serialize]" )
{
  GenericContainer gc;
  gc = vec_real_type{ 1.5, -2.5 };

  int32_t const        need = gc.mem_size();
  std::vector<uint8_t> buf( size_t( need ) + 32 );
  CHECK( gc.serialize( int32_t( buf.size() ), buf.data() ) == need );
  CHECK( gc.serialize( need, buf.data() ) == need );

  GenericContainer dst;
  CHECK( dst.de_serialize( need, buf.data() ) == need );
  CHECK( gc_equal( dst, gc ) );
}

TEST_CASE( "serialize into undersized buffer throws", "[serialize][errors]" )
{
  GenericContainer gc;
  gc = vec_real_type{ 1.5, -2.5 };
  int32_t const        need = gc.mem_size();
  std::vector<uint8_t> buf( static_cast<size_t>( need ) );
  CHECK_THROWS_AS( gc.serialize( need - 1, buf.data() ), std::runtime_error );
  CHECK_THROWS_AS( gc.serialize( 0, buf.data() ), std::runtime_error );
}

TEST_CASE( "span overloads serialize and de_serialize", "[serialize][span]" )
{
  GenericContainer gc;
  gc["x"] = vec_real_type{ 1.5, 2.5 };

  std::vector<uint8_t> buf( size_t( gc.mem_size() ) );
  CHECK( gc.serialize( std::span<uint8_t>( buf ) ) == int32_t( buf.size() ) );

  GenericContainer dst;
  CHECK( dst.de_serialize( std::span<uint8_t const>( buf ) ) == int32_t( buf.size() ) );
  CHECK( dst == gc );
  CHECK_FALSE( dst != gc );
}

TEST_CASE( "property: random containers round trip", "[serialize][property]" )
{
  std::mt19937 rng( 20260702u );  // fixed seed: deterministic corpus
  for ( int k = 0; k < 200; ++k )
  {
    GenericContainer const gc{ make_random_gc( rng ) };
    GenericContainer const back{ round_trip( gc ) };
    INFO( "iteration " << k << " type " << gc.get_type_name() );
    CHECK( gc_equal( back, gc ) );
  }
}

TEST_CASE( "wire format golden pin", "[serialize][golden]" )
{
  // Captured from the pre-rewrite (tagged-union) implementation. The wire
  // format is little-endian with int32 tags/sizes and column-major matrix
  // payloads; it must stay byte-identical through the variant and Eigen
  // migrations.
  static char const golden_hex[] =
    "140000001200000005000000626f6f6c00020000000108000000636f6d706c657800060000000000"
    "00000000f03f00000000000000c004000000696e7400030000002a000000050000006c6f6e670004"
    "0000000000000000010000040000006d617000140000000200000005000000646565700014000000"
    "01000000050000006c656166000a00000002000000090000000800000006000000696e6e65720005"
    "000000000000000000f43f0c0000006d61745f636f6d706c65780012000000010000000200000000"
    "0000000000f03f000000000000f0bf00000000000000c00000000000000040080000006d61745f69"
    "6e74000f0000000200000003000000000000000a000000010000000b000000020000000c00000009"
    "0000006d61745f6c6f6e670010000000020000000200000000000000080000000a00000008000000"
    "01000000080000000b00000008000000090000006d61745f7265616c001100000002000000020000"
    "00000000000000d03f000000000000e03f000000000000e83f000000000000f03f05000000726561"
    "6c00050000000000000000000c4007000000737472696e6700070000000600000068656c6c6f0009"
    "0000007665635f626f6f6c0009000000030000000100010c0000007665635f636f6d706c6578000d"
    "00000002000000000000000000f03f000000000000004000000000000008c0000000000000104008"
    "0000007665635f696e74000a0000000300000001000000feffffff03000000090000007665635f6c"
    "6f6e67000b00000002000000000000000200000000000000fcffffff090000007665635f7265616c"
    "000c00000003000000000000000000e03f000000000000f83f00000000000004c00b000000766563"
    "5f737472696e67000e00000003000000020000006100030000006262000400000063636300070000"
    "00766563746f72001300000003000000030000000700000007000000070000006e65737465640002"
    "00000000";

  std::vector<uint8_t> golden;
  for ( char const * p = golden_hex; p[0] != '\0' && p[1] != '\0'; p += 2 )
  {
    auto nibble = []( char const c ) -> uint8_t
    { return uint8_t( c <= '9' ? c - '0' : c - 'a' + 10 ); };
    golden.push_back( uint8_t( ( nibble( p[0] ) << 4 ) | nibble( p[1] ) ) );
  }
  REQUIRE( golden.size() == 724 );

  GenericContainer const gc{ make_maximal_gc() };
  CHECK( gc.serialize() == golden );

  GenericContainer decoded;
  decoded.de_serialize( golden );
  CHECK( gc_equal( decoded, gc ) );
}

TEST_CASE( "truncated buffers throw instead of crashing", "[serialize][errors]" )
{
  GenericContainer gc{ make_maximal_gc() };
  std::vector<uint8_t> const buffer{ gc.serialize() };

  // Every proper prefix must be detected as exhausted/corrupted.
  for ( size_t len{ 0 }; len < buffer.size(); ++len )
  {
    GenericContainer dst;
    INFO( "prefix length " << len << " of " << buffer.size() );
    CHECK_THROWS_AS( dst.de_serialize( int32_t( len ), buffer.data() ), std::runtime_error );
  }
}

TEST_CASE( "corrupted type tag is rejected", "[serialize][errors][newbehavior]" )
{
  GenericContainer gc;
  gc.set_int( 7 );
  std::vector<uint8_t> buffer{ gc.serialize() };
  buffer[0] = 0xFF;  // clobber the tag (first byte of the little-endian int32)
  GenericContainer dst;
  CHECK_THROWS_AS( dst.de_serialize( int32_t( buffer.size() ), buffer.data() ), std::runtime_error );
}

TEST_CASE( "oversized matrix dims are rejected before allocating", "[serialize][errors]" )
{
  // A forged header claiming a 2^30 x 2^30 real matrix must be refused by the
  // payload-vs-remaining-bytes pre-check, not attempted as a huge allocation.
  auto put_i32 = []( std::vector<uint8_t> & b, int32_t v )
  {
    for ( int k = 0; k < 4; ++k ) b.push_back( uint8_t( ( uint32_t( v ) >> ( 8 * k ) ) & 0xFF ) );
  };
  std::vector<uint8_t> forged;
  put_i32( forged, int32_t( GC_type::MAT_REAL ) );
  put_i32( forged, 1 << 30 );  // nr
  put_i32( forged, 1 << 30 );  // nc
  forged.resize( forged.size() + 64, 0 );

  GenericContainer dst;
  CHECK_THROWS_AS( dst.de_serialize( int32_t( forged.size() ), forged.data() ), std::runtime_error );

  // same for a forged vector length
  std::vector<uint8_t> forged_vec;
  put_i32( forged_vec, int32_t( GC_type::VEC_REAL ) );
  put_i32( forged_vec, 1 << 30 );
  forged_vec.resize( forged_vec.size() + 64, 0 );
  GenericContainer dst2;
  CHECK_THROWS_AS( dst2.de_serialize( int32_t( forged_vec.size() ), forged_vec.data() ), std::runtime_error );
}
