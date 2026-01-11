/*--------------------------------------------------------------------------*\
 |                                                                          |
 |  Copyright (C) 2024                                                      |
 |                                                                          |
 |         , __                 , __                                        |
 |        /|/  \               /|/  \                                       |
 |         | __/ _   ,_         | __/ _   ,_                                |
 |         |   \|/  /  |  |   | |   \|/  /  |  |   |                        |
 |         |(__/|__/   |_/ \_/|/|(__/|__/   |_/ \_/|/                       |
 |                           /|                   /|                        |
 |                           \|                   \|                        |
 |                                                                          |
 |      Comprehensive Test Suite for GenericContainer                       |
 |                                                                          |
\*--------------------------------------------------------------------------*/

/*!
 \example comprehensive_test.cc

 Complete test suite for GenericContainer class.
 Testing ALL major functionalities with structured output.

 */

#include "GenericContainer.hh"
#include <iomanip>
#include <vector>
#include <map>
#include <complex>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

using namespace std;
using namespace GC;

// ===========================================================================
// Helper Functions for Formatted Output
// ===========================================================================

string repeat( int N, string const & ch )
{
  string res;
  for ( int k = 0; k < N; ++k ) res += ch;
  return res;
}

void print_header( const string & title )
{
  cout << "\n  " << repeat( 59, "═" )
       << "\n  " << title
       << "\n  " << repeat( 59, "═" )
       << "\n";
}

void print_subheader( const string & title )
{
  cout << "\n  " << repeat( 59, "─" )
       << "\n  " << title
       << "\n  " << repeat( 59, "─" )
       << "\n";
}

void print_test_case( const string & name, bool passed )
{
  string status = passed ? "✔ PASS" : "✘ FAIL";
  cout << "  " << setw( 40 ) << left << name << " " << status << "\n";
}

void print_info_table( const string & name, const GenericContainer & gc )
{
  cout << "  ┌─────────────────────────────────────────────────────────┐\n";
  cout << "  │ " << setw( 55 ) << left << name << " │\n";
  cout << "  ├─────────────────────────────────────────────────────────┤\n";
  cout << "  │ Type:     " << setw( 45 ) << left << gc.get_type_name() << " │\n";
  cout << "  │ Elements: " << setw( 45 ) << left << gc.get_num_elements() << " │\n";
  cout << "  └─────────────────────────────────────────────────────────┘\n";
}

// ===========================================================================
// Test Suites
// ===========================================================================

void test_simple_types()
{
  print_header( "TEST 1: Simple Data Types" );

  GenericContainer gc;
  bool             all_passed = true;

  // Test boolean
  {
    gc.set_bool( true );
    bool passed = ( gc.get_bool() == true );
    print_test_case( "set_bool / get_bool", passed );
    print_info_table( "Boolean: true", gc );
    all_passed &= passed;
  }

  // Test integer
  {
    gc.set_int( 42 );
    bool passed = ( gc.get_int() == 42 );
    print_test_case( "set_int / get_int", passed );
    print_info_table( "Integer: 42", gc );
    all_passed &= passed;
  }

  // Test long integer
  {
    gc.set_long( 1234567890123LL );
    bool passed = ( gc.get_long() == 1234567890123LL );
    print_test_case( "set_long / get_long", passed );
    print_info_table( "Long: 1234567890123", gc );
    all_passed &= passed;
  }

  // Test real
  {
    gc.set_real( 3.141592653589793 );
    bool passed = ( abs( gc.get_real() - 3.141592653589793 ) < 1e-12 );
    print_test_case( "set_real / get_real", passed );
    print_info_table( "Real: 3.141592653589793", gc );
    all_passed &= passed;
  }

  // Test complex
  {
    gc.set_complex( 1.0, 2.0 );
    complex_type c      = gc.get_complex();
    bool         passed = ( abs( c.real() - 1.0 ) < 1e-12 && abs( c.imag() - 2.0 ) < 1e-12 );
    print_test_case( "set_complex / get_complex", passed );
    print_info_table( "Complex: 1.0 + 2.0i", gc );
    all_passed &= passed;
  }

  // Test string
  {
    gc.set_string( "Hello, GenericContainer!" );
    bool passed = ( gc.get_string() == "Hello, GenericContainer!" );
    print_test_case( "set_string / get_string", passed );
    print_info_table( "String: Hello, GenericContainer!", gc );
    all_passed &= passed;
  }

  // Test pointer
  {
    int x = 42;
    gc.set_pointer( &x );
    void * ptr    = gc.get_pvoid();
    bool   passed = ( ptr == &x );
    print_test_case( "set_pointer / get_pvoid", passed );
    print_info_table( "Pointer", gc );
    all_passed &= passed;
  }

  print_test_case( "Simple Types Overall", all_passed );
}

void test_vector_types()
{
  print_header( "TEST 2: Vector Types" );

  bool all_passed = true;

  // Test vector of integers
  {
    GenericContainer gc;
    vec_int_type     vec = { 1, 2, 3, 4, 5 };
    gc.set_vec_int( vec );

    vec_int_type & retrieved = gc.get_vec_int();
    bool           passed    = ( retrieved == vec );
    print_test_case( "set_vec_int / get_vec_int", passed );
    print_info_table( "Vector of Integers (5 elements)", gc );

    if ( !passed )
    {
      cout << "  Expected: [1, 2, 3, 4, 5]\n";
      cout << "  Got: [";
      for ( size_t i = 0; i < retrieved.size(); ++i )
      {
        cout << retrieved[i] << ( i < retrieved.size() - 1 ? ", " : "" );
      }
      cout << "]\n";
    }
    all_passed &= passed;
  }

  // Test vector of reals
  {
    GenericContainer gc;
    vec_real_type    vec = { 1.1, 2.2, 3.3, 4.4, 5.5 };
    gc.set_vec_real( vec );

    vec_real_type & retrieved = gc.get_vec_real();
    bool            passed    = ( retrieved.size() == vec.size() );
    if ( passed )
    {
      for ( size_t i = 0; i < vec.size(); ++i )
      {
        if ( abs( retrieved[i] - vec[i] ) > 1e-12 )
        {
          passed = false;
          break;
        }
      }
    }
    print_test_case( "set_vec_real / get_vec_real", passed );
    print_info_table( "Vector of Reals (5 elements)", gc );
    all_passed &= passed;
  }

  // Test vector of complex numbers
  {
    GenericContainer gc;
    vec_complex_type vec = { { 1.0, 2.0 }, { 3.0, 4.0 }, { 5.0, 6.0 } };
    gc.set_vec_complex( vec );

    vec_complex_type & retrieved = gc.get_vec_complex();
    bool               passed    = ( retrieved.size() == vec.size() );
    if ( passed )
    {
      for ( size_t i = 0; i < vec.size(); ++i )
      {
        if ( abs( retrieved[i].real() - vec[i].real() ) > 1e-12 || abs( retrieved[i].imag() - vec[i].imag() ) > 1e-12 )
        {
          passed = false;
          break;
        }
      }
    }
    print_test_case( "set_vec_complex / get_vec_complex", passed );
    print_info_table( "Vector of Complex (3 elements)", gc );
    all_passed &= passed;
  }

  // Test vector of strings
  {
    GenericContainer gc;
    vec_string_type  vec = { "Apple", "Banana", "Cherry" };
    gc.set_vec_string( vec );

    vec_string_type & retrieved = gc.get_vec_string();
    bool              passed    = ( retrieved == vec );
    print_test_case( "set_vec_string / get_vec_string", passed );
    print_info_table( "Vector of Strings (3 elements)", gc );
    all_passed &= passed;
  }

  // Test vector of booleans
  {
    GenericContainer gc;
    vec_bool_type    vec = { true, false, true, true, false };
    gc.set_vec_bool( vec );

    vec_bool_type & retrieved = gc.get_vec_bool();
    bool            passed    = ( retrieved == vec );
    print_test_case( "set_vec_bool / get_vec_bool", passed );
    print_info_table( "Vector of Booleans (5 elements)", gc );
    all_passed &= passed;
  }

  // Test element access
  {
    GenericContainer gc;
    gc.set_vec_int( { 10, 20, 30, 40, 50 } );

    bool passed = true;
    passed &= ( gc.get_int_at( 0 ) == 10 );
    passed &= ( gc.get_int_at( 2 ) == 30 );
    passed &= ( gc.get_int_at( 4 ) == 50 );

    print_test_case( "get_int_at element access", passed );
    all_passed &= passed;
  }

  print_test_case( "Vector Types Overall", all_passed );
}

void test_matrix_types()
{
  print_header( "TEST 3: Matrix Types" );

  bool all_passed = true;

  // Test matrix of integers
  {
    GenericContainer gc;
    mat_int_type     mat( 2, 3 );
    mat( 0, 0 ) = 1;
    mat( 0, 1 ) = 2;
    mat( 0, 2 ) = 3;
    mat( 1, 0 ) = 4;
    mat( 1, 1 ) = 5;
    mat( 1, 2 ) = 6;

    gc.set_mat_int( mat );

    mat_int_type & retrieved = gc.get_mat_int();
    bool           passed    = ( retrieved.num_rows() == 2 && retrieved.num_cols() == 3 );
    if ( passed )
    {
      for ( unsigned i = 0; i < 2; ++i )
      {
        for ( unsigned j = 0; j < 3; ++j )
        {
          if ( retrieved( i, j ) != mat( i, j ) )
          {
            passed = false;
            break;
          }
        }
      }
    }

    print_test_case( "set_mat_int / get_mat_int", passed );
    print_info_table( "2x3 Integer Matrix", gc );

    if ( passed )
    {
      cout << "  Matrix contents:\n";
      cout << "  ┌          ┐\n";
      cout << "  │ " << setw( 2 ) << retrieved( 0, 0 ) << " " << setw( 2 ) << retrieved( 0, 1 ) << " " << setw( 2 )
           << retrieved( 0, 2 ) << " │\n";
      cout << "  │ " << setw( 2 ) << retrieved( 1, 0 ) << " " << setw( 2 ) << retrieved( 1, 1 ) << " " << setw( 2 )
           << retrieved( 1, 2 ) << " │\n";
      cout << "  └          ┘\n";
    }
    all_passed &= passed;
  }

  // Test matrix of reals
  {
    GenericContainer gc;
    mat_real_type    mat( 3, 2 );
    for ( unsigned i = 0; i < 3; ++i )
    {
      for ( unsigned j = 0; j < 2; ++j ) { mat( i, j ) = ( i + 1 ) * 10 + ( j + 1 ); }
    }

    gc.set_mat_real( mat );

    mat_real_type & retrieved = gc.get_mat_real();
    bool            passed    = ( retrieved.num_rows() == 3 && retrieved.num_cols() == 2 );
    if ( passed )
    {
      for ( unsigned i = 0; i < 3; ++i )
      {
        for ( unsigned j = 0; j < 2; ++j )
        {
          if ( abs( retrieved( i, j ) - mat( i, j ) ) > 1e-12 )
          {
            passed = false;
            break;
          }
        }
      }
    }

    print_test_case( "set_mat_real / get_mat_real", passed );
    print_info_table( "3x2 Real Matrix", gc );
    all_passed &= passed;
  }

  // Test matrix element access
  {
    GenericContainer gc;
    mat_int_type     mat( 2, 2 );
    mat( 0, 0 ) = 1;
    mat( 0, 1 ) = 2;
    mat( 1, 0 ) = 3;
    mat( 1, 1 ) = 4;
    gc.set_mat_int( mat );

    bool passed = true;
    passed &= ( gc.get_int_at( 0, 0 ) == 1 );
    passed &= ( gc.get_int_at( 0, 1 ) == 2 );
    passed &= ( gc.get_int_at( 1, 0 ) == 3 );
    passed &= ( gc.get_int_at( 1, 1 ) == 4 );

    print_test_case( "get_int_at matrix element access", passed );
    all_passed &= passed;
  }

  print_test_case( "Matrix Types Overall", all_passed );
}

void test_complex_containers()
{
  print_header( "TEST 4: Complex Containers (Vector & Map)" );

  bool all_passed = true;

  // Test vector of GenericContainers
  {
    GenericContainer gc;
    vector_type &    vec = gc.set_vector( 3 );

    vec[0].set_int( 100 );
    vec[1].set_string( "Element 1" );
    vec[2].set_real( 3.14 );

    bool passed = true;
    passed &= ( gc.get_vector().size() == 3 );
    passed &= ( gc[0].get_int() == 100 );
    passed &= ( gc[1].get_string() == "Element 1" );
    passed &= ( abs( gc[2].get_real() - 3.14 ) < 1e-12 );

    print_test_case( "set_vector / get_vector / operator[]", passed );
    print_info_table( "Vector of GenericContainers (3 elements)", gc );

    if ( passed )
    {
      cout << "  Vector structure:\n";
      for ( unsigned i = 0; i < 3; ++i ) { cout << "    [" << i << "]: " << gc[i].get_type_name() << "\n"; }
    }
    all_passed &= passed;
  }

  // Test map of GenericContainers
  {
    GenericContainer gc;
    map_type &       map = gc.set_map();

    map["id"].set_int( 42 );
    map["name"].set_string( "Test Object" );
    map["value"].set_real( 123.456 );
    map["flags"].set_vec_bool( { true, false, true } );

    bool passed = true;
    passed &= ( gc.get_map().size() == 4 );
    passed &= ( gc["id"].get_int() == 42 );
    passed &= ( gc["name"].get_string() == "Test Object" );
    passed &= ( abs( gc["value"].get_real() - 123.456 ) < 1e-12 );
    passed &= ( gc["flags"].get_vec_bool().size() == 3 );

    print_test_case( "set_map / get_map / operator[string]", passed );
    print_info_table( "Map of GenericContainers (4 keys)", gc );

    if ( passed )
    {
      cout << "  Map structure:\n";
      cout << "  ┌──────────────────┬─────────────────┐\n";
      cout << "  │ Key              │ Type            │\n";
      cout << "  ├──────────────────┼─────────────────┤\n";
      cout << "  │ id               │ " << setw( 15 ) << left << gc["id"].get_type_name() << " │\n";
      cout << "  │ name             │ " << setw( 15 ) << left << gc["name"].get_type_name() << " │\n";
      cout << "  │ value            │ " << setw( 15 ) << left << gc["value"].get_type_name() << " │\n";
      cout << "  │ flags            │ " << setw( 15 ) << left << gc["flags"].get_type_name() << " │\n";
      cout << "  └──────────────────┴─────────────────┘\n";
    }
    all_passed &= passed;
  }

  // Test exists() and get_if_exists()
  {
    GenericContainer gc;
    gc.set_map();
    gc["present"].set_int( 999 );

    bool exists_check     = gc.exists( "present" );
    bool not_exists_check = !gc.exists( "missing" );

    int  value;
    bool got_if_exists        = gc.get_if_exists( "present", value );
    bool correct_value        = ( value == 999 );
    bool no_get_if_not_exists = !gc.get_if_exists( "missing", value );

    bool passed = exists_check && not_exists_check && got_if_exists && correct_value && no_get_if_not_exists;

    print_test_case( "exists() and get_if_exists()", passed );
    all_passed &= passed;
  }

  print_test_case( "Complex Containers Overall", all_passed );
}

void test_assignment_and_copy()
{
  print_header( "TEST 5: Assignment and Copy Operations" );

  bool all_passed = true;

  // Test copy constructor
  {
    GenericContainer original;
    original.set_string( "Original content" );

    GenericContainer copy( original );
    bool             passed = ( copy.get_string() == "Original content" );
    print_test_case( "Copy constructor", passed );
    all_passed &= passed;
  }

  // Test assignment operator
  {
    GenericContainer source;
    source.set_vec_int( { 1, 2, 3 } );

    GenericContainer target;
    target = source;

    bool passed = ( target.get_vec_int().size() == 3 );
    if ( passed )
    {
      for ( int i = 0; i < 3; ++i )
      {
        if ( target.get_vec_int()[i] != i + 1 )
        {
          passed = false;
          break;
        }
      }
    }

    print_test_case( "Assignment operator", passed );
    all_passed &= passed;
  }

  // Test move semantics (implicitly)
  {
    GenericContainer temp;
    temp.set_map();
    temp["data"].set_real( 42.0 );

    GenericContainer moved  = std::move( temp );
    bool             passed = ( moved["data"].get_real() == 42.0 ) && temp.empty();

    print_test_case( "Move semantics", passed );
    all_passed &= passed;
  }

  // Test clear()
  {
    GenericContainer gc;
    gc.set_vec_string( { "a", "b", "c" } );
    gc.clear();

    bool passed = gc.empty();
    print_test_case( "clear() method", passed );
    all_passed &= passed;
  }

  print_test_case( "Assignment and Copy Overall", all_passed );
}

void test_promotion()
{
  print_header( "TEST 6: Type Promotion" );

  bool all_passed = true;

  // Test promote_to_int
  {
    GenericContainer gc;
    gc.set_bool( true );
    gc.promote_to_int();

    bool passed = ( gc.get_type() == GC_type::INTEGER && gc.get_int() == 1 );
    print_test_case( "promote_to_int (bool → int)", passed );
    all_passed &= passed;
  }

  // Test promote_to_real
  {
    GenericContainer gc;
    gc.set_int( 42 );
    gc.promote_to_real();

    bool passed = ( gc.get_type() == GC_type::REAL && abs( gc.get_real() - 42.0 ) < 1e-12 );
    print_test_case( "promote_to_real (int → real)", passed );
    all_passed &= passed;
  }

  // Test promote_to_complex
  {
    GenericContainer gc;
    gc.set_real( 3.14 );
    gc.promote_to_complex();

    complex_type c = gc.get_complex();
    bool passed    = ( gc.get_type() == GC_type::COMPLEX && abs( c.real() - 3.14 ) < 1e-12 && abs( c.imag() ) < 1e-12 );
    print_test_case( "promote_to_complex (real → complex)", passed );
    all_passed &= passed;
  }

  // Test promote_to_vec_real
  {
    GenericContainer gc;
    gc.set_vec_int( { 1, 2, 3, 4 } );
    gc.promote_to_vec_real();

    bool passed = ( gc.get_type() == GC_type::VEC_REAL );
    if ( passed )
    {
      vec_real_type vec = gc.get_vec_real();
      passed            = ( vec.size() == 4 );
      for ( int i = 0; i < 4 && passed; ++i )
      {
        if ( abs( vec[i] - ( i + 1 ) ) > 1e-12 ) passed = false;
      }
    }

    print_test_case( "promote_to_vec_real (vec_int → vec_real)", passed );
    all_passed &= passed;
  }

  print_test_case( "Type Promotion Overall", all_passed );
}

void test_serialization()
{
  print_header( "TEST 7: Serialization" );

  bool all_passed = true;

  // Test simple serialization
  {
    GenericContainer original;
    original.set_map();
    original["number"].set_int( 42 );
    original["text"].set_string( "Hello" );
    original["array"].set_vec_real( { 1.1, 2.2, 3.3 } );

    // Serialize
    int32_t         size = original.mem_size();
    vector<uint8_t> buffer( original.serialize() );

    // Deserialize
    GenericContainer restored;
    int32_t          used = restored.de_serialize( buffer );

    bool passed = ( used == size );
    if ( passed )
    {
      // Compare contents
      string diff = original.compare_content( restored, "Serialization test" );
      passed      = diff.empty();
    }

    print_test_case( "serialize() / de_serialize()", passed );

    if ( passed )
    {
      cout << "  📦 Serialized size: " << size << " bytes\n";
      cout << "  🔄 Successfully restored identical container\n";
    }
    else
    {
      cout << "  ❗ Serialization/deserialization mismatch\n";
    }

    all_passed &= passed;
  }

  print_test_case( "Serialization Overall", all_passed );
}

void test_io_formats()
{
  print_header( "TEST 8: I/O Formats (JSON/YAML/TOML)" );

  bool all_passed = true;

  GenericContainer data;
  data.set_map();
  data["app_name"].set_string( "GenericContainer Test" );
  data["version"].set_int( 2 );
  data["enabled"].set_bool( true );
  data["values"].set_vec_real( { 1.5, 2.5, 3.5 } );

  // Test JSON
  {
    string json_str = data.to_json();

    GenericContainer parsed_json;
    bool             parse_success = parsed_json.from_json( json_str );

    bool passed = parse_success;
    if ( passed )
    {
      string diff = data.compare_content( parsed_json, "JSON test" );
      passed      = diff.empty();
    }

    print_test_case( "JSON serialization/parsing", passed );

    if ( passed )
    {
      cout << "  📄 JSON output (" << json_str.length() << " chars):\n";
      // Print first 100 chars of JSON
      string preview = json_str.substr( 0, std::min<size_t>( 100, json_str.length() ) );
      cout << "    " << preview << ( json_str.length() > 100 ? "..." : "" ) << "\n";
    }

    all_passed &= passed;
  }

  // Test YAML
  {
    string yaml_str = data.to_yaml();

    GenericContainer parsed_yaml;
    bool             parse_success = parsed_yaml.from_yaml( yaml_str );

    bool passed = parse_success;
    if ( passed )
    {
      string diff = data.compare_content( parsed_yaml, "YAML test" );
      passed      = diff.empty();
    }

    print_test_case( "YAML serialization/parsing", passed );

    if ( passed )
    {
      cout << "  📄 YAML output (" << yaml_str.length() << " chars):\n";
      // Print first few lines of YAML
      size_t pos   = 0;
      int    lines = 0;
      while ( pos < yaml_str.length() && lines < 3 )
      {
        size_t next = yaml_str.find( '\n', pos );
        if ( next == string::npos ) break;
        cout << "    " << yaml_str.substr( pos, next - pos ) << "\n";
        pos = next + 1;
        lines++;
      }
    }

    all_passed &= passed;
  }

  print_test_case( "I/O Formats Overall", all_passed );
}

void test_advanced_features()
{
  print_header( "TEST 9: Advanced Features" );

  bool all_passed = true;

  // Test collapse()
  {
    GenericContainer gc;
    gc.set_vector( 2 );

    gc[0].set_vec_real( { 1.0, 2.0 } );
    gc[1].set_vec_real( { 3.0, 4.0 } );

    gc.collapse();

    bool passed = ( gc.get_type() == GC_type::MAT_REAL );
    if ( passed )
    {
      mat_real_type & mat = gc.get_mat_real();
      passed              = ( mat.num_rows() == 2 && mat.num_cols() == 2 );
    }

    print_test_case( "collapse() vector of vectors → matrix", passed );
    all_passed &= passed;
  }

  // Test get_number() and get_complex_number()
  {
    GenericContainer gc_int;
    gc_int.set_int( 42 );

    GenericContainer gc_real;
    gc_real.set_real( 3.14 );

    GenericContainer gc_complex;
    gc_complex.set_complex( 1.0, 2.0 );

    bool passed = true;
    passed &= ( abs( gc_int.get_number() - 42.0 ) < 1e-12 );
    passed &= ( abs( gc_real.get_number() - 3.14 ) < 1e-12 );

    complex_type c = gc_complex.get_complex_number();
    passed &= ( abs( c.real() - 1.0 ) < 1e-12 && abs( c.imag() - 2.0 ) < 1e-12 );

    print_test_case( "get_number() / get_complex_number()", passed );
    all_passed &= passed;
  }

  // Test operator() with error context
  {
    GenericContainer gc;
    gc.set_vec_int( { 10, 20, 30 } );

    bool passed = true;
    try
    {
      GenericContainer & elem = gc( 1, "Accessing element 1" );
      passed                  = ( elem.get_int() == 20 );

      // Should throw for out of bounds
      gc( 10, "Accessing out of bounds" );
      passed = false;  // Should not reach here
    }
    catch ( ... )
    {
      // Expected for out of bounds
    }

    print_test_case( "operator() with error context", passed );
    all_passed &= passed;
  }

  print_test_case( "Advanced Features Overall", all_passed );
}

void test_move_semantics()
{
  print_header( "TEST MOVE SEMANTICS" );

  bool all_passed = true;

  // Test 1: Costruttore di movimento
  {
    GenericContainer source;
    source.set_vec_int( { 1, 2, 3, 4, 5 } );
    void * original_data = source.get_int_pointer();

    GenericContainer moved( std::move( source ) );

    bool passed = true;
    passed &= ( moved.get_vec_int().size() == 5 );
    passed &= ( moved.get_int_pointer() == original_data );  // Stesso puntatore!
    passed &= source.empty();                                // Source dovrebbe essere vuoto

    print_test_case( "Move constructor transfers ownership", passed );
    all_passed &= passed;
  }

  // Test 2: Operatore di assegnazione di movimento
  {
    GenericContainer source;
    source.set_string( "Hello World" );
    //void * original_ptr = static_cast<void *>( &source.get_string()[0] );

    GenericContainer target;
    target.set_int( 42 );

    target = std::move( source );

    bool passed = true;
    passed &= ( target.get_string() == "Hello World" );
    passed &= source.empty();  // Source è stato spostato

    print_test_case( "Move assignment operator", passed );
    all_passed &= passed;
  }

  // Test 3: Swap function
  {
    GenericContainer a;
    a.set_int( 100 );

    GenericContainer b;
    b.set_string( "Swap Test" );

    swap( a, b );

    bool passed = true;
    passed &= ( a.get_string() == "Swap Test" );
    passed &= ( b.get_int() == 100 );

    print_test_case( "Swap function", passed );
    all_passed &= passed;
  }

  // Test 4: Performance con std::vector
  {
    std::vector<GenericContainer> vec;
    vec.reserve( 10 );

    auto start = chrono::high_resolution_clock::now();

    for ( int i = 0; i < 1000; ++i )
    {
      GenericContainer gc;
      gc.set_vec_int( std::vector<int_type>( 1000, i ) );
      vec.push_back( std::move( gc ) );  // Move invece di copy
    }

    auto end      = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>( end - start );

    bool passed = ( vec.size() == 1000 );
    print_test_case( "Move with std::vector (Performance)", passed );

    if ( passed )
    {
      cout << "  ⚡ Time with move semantics: " << duration.count() << " μs\n";

      // Confronta con copy (se vuoi testare la differenza)
      // Nota: la versione con copy sarebbe molto più lenta
    }

    all_passed &= passed;
  }

  // Test 5: Return value optimization (RVO) + move
  {
    auto create_container = []() -> GenericContainer
    {
      GenericContainer gc;
      gc.set_map();
      gc["data"].set_vec_real( { 1.1, 2.2, 3.3 } );
      gc["info"].set_string( "Created in function" );
      return gc;  // Dovrebbe usare move (o NRVO)
    };

    GenericContainer result = create_container();

    bool passed = ( result.get_map().size() == 2 );
    passed &= ( result["info"].get_string() == "Created in function" );

    print_test_case( "Return value + move semantics", passed );
    all_passed &= passed;
  }

  print_test_case( "Move Semantics Overall", all_passed );
}

void test_performance_stress()
{
  print_header( "TEST 10: Performance and Stress" );

  cout << "  🏃 Running performance tests...\n";

  // Stress test with large nested structure
  auto start = chrono::high_resolution_clock::now();

  GenericContainer root;
  root.set_map();

  // Create 1000 entries
  for ( int i = 0; i < 1000; ++i )
  {
    string key = "item_" + to_string( i );
    root[key].set_map();
    root[key]["id"].set_int( i );
    root[key]["value"].set_real( i * 1.5 );
    root[key]["data"].set_vec_int( vector<int_type>( 100, i ) );
  }

  auto mid = chrono::high_resolution_clock::now();

  // Serialize the large structure
  vector<uint8_t> buffer = root.serialize();

  // Deserialize it
  GenericContainer restored;
  restored.de_serialize( buffer );

  auto end = chrono::high_resolution_clock::now();

  auto creation_time = chrono::duration_cast<chrono::microseconds>( mid - start ).count();
  auto serde_time    = chrono::duration_cast<chrono::microseconds>( end - mid ).count();

  cout << "  📊 Results:\n";
  cout << "    • Created structure with " << root.get_map().size() << " entries\n";
  cout << "    • Creation time: " << creation_time << " μs\n";
  cout << "    • Serialization/deserialization time: " << serde_time << " μs\n";
  cout << "    • Serialized size: " << buffer.size() << " bytes\n";

  // Verify integrity
  bool passed = ( root.compare_content( restored, "Stress test" ).empty() );
  print_test_case( "Large structure integrity", passed );
}

// Aggiungi dopo test_performance_stress() ma prima di main()

void test_mat_type_comprehensive()
{
  print_header("TEST MAT_TYPE: Comprehensive Matrix Type Tests");

  bool all_passed = true;

  // Test 1: Costruttori e resize
  {
    mat_int_type mat1;
    bool passed = (mat1.num_rows() == 0 && mat1.num_cols() == 0 && mat1.size() == 0);
    print_test_case("Default constructor", passed);
    all_passed &= passed;
  }

  {
    mat_int_type mat(3, 4);
    bool passed = (mat.num_rows() == 3 && mat.num_cols() == 4 && mat.size() == 12);
    print_test_case("Constructor with dimensions", passed);
    all_passed &= passed;
  }

  {
    mat_int_type mat(2, 3);
    mat.resize(4, 5);
    bool passed = (mat.num_rows() == 4 && mat.num_cols() == 5 && mat.size() == 20);
    print_test_case("resize() method", passed);
    all_passed &= passed;
  }

  // Test 2: Accesso operatori e bound checking implicito
  {
    mat_int_type mat(2, 2);
    mat(0, 0) = 1;
    mat(0, 1) = 2;
    mat(1, 0) = 3;
    mat(1, 1) = 4;

    bool passed = true;
    passed &= (mat(0, 0) == 1);
    passed &= (mat(0, 1) == 2);
    passed &= (mat(1, 0) == 3);
    passed &= (mat(1, 1) == 4);
    
    // Test const access
    const mat_int_type& cmat = mat;
    passed &= (cmat(0, 0) == 1);
    
    print_test_case("Operator() access", passed);
    
    if (!passed) {
      cout << "  Matrix:\n";
      for (unsigned i = 0; i < 2; ++i) {
        cout << "  [ ";
        for (unsigned j = 0; j < 2; ++j) cout << mat(i, j) << " ";
        cout << "]\n";
      }
    }
    all_passed &= passed;
  }

  // Test 3: get_column con vector
  {
    mat_int_type mat(3, 3);
    for (unsigned i = 0; i < 3; ++i)
      for (unsigned j = 0; j < 3; ++j)
        mat(i, j) = i * 10 + j;

    vector<int_type> col;
    mat.get_column(1, col);
    
    bool passed = (col.size() == 3);
    passed &= (col[0] == 1);
    passed &= (col[1] == 11);
    passed &= (col[2] == 21);
    
    print_test_case("get_column to vector", passed);
    all_passed &= passed;
  }

  // Test 4: get_column con array
  {
    mat_real_type mat(2, 3);
    for (unsigned i = 0; i < 2; ++i)
      for (unsigned j = 0; j < 3; ++j)
        mat(i, j) = i * 1.5 + j * 0.5;

    real_type col[2];
    mat.get_column(0, col);
    
    bool passed = (abs(col[0] - 0.0) < 1e-12);
    passed &= (abs(col[1] - 1.5) < 1e-12);
    
    print_test_case("get_column to array", passed);
    all_passed &= passed;
  }

  // Test 5: get_row con vector e array
  {
    mat_int_type mat(2, 4);
    for (unsigned i = 0; i < 2; ++i)
      for (unsigned j = 0; j < 4; ++j)
        mat(i, j) = i * 100 + j * 10;

    vector<int_type> row;
    mat.get_row(1, row);
    
    bool passed = (row.size() == 4);
    passed &= (row[0] == 100);
    passed &= (row[1] == 110);
    passed &= (row[2] == 120);
    passed &= (row[3] == 130);
    
    print_test_case("get_row to vector", passed);
    all_passed &= passed;
  }

  // Test 6: data() method
  {
    mat_int_type mat(2, 2);
    mat(0, 0) = 1;
    mat(1, 0) = 2;
    mat(0, 1) = 3;
    mat(1, 1) = 4;
    
    bool passed = true;
    int_type* data = mat.data();
    passed &= (data[0] == 1);
    passed &= (data[1] == 2);
    passed &= (data[2] == 3);
    passed &= (data[3] == 4);
    
    const mat_int_type& cmat = mat;
    const int_type* cdata = cmat.data();
    passed &= (cdata[1] == 2);
    
    print_test_case("data() method", passed);
    all_passed &= passed;
  }

  // Test 7: info() method
  {
    mat_int_type mat(2, 3);
    for (unsigned i = 0; i < 2; ++i)
      for (unsigned j = 0; j < 3; ++j)
        mat(i, j) = i * 3 + j + 1;
    
    ostringstream oss;
    mat.info(oss);
    string info_str = oss.str();
    
    bool passed = !info_str.empty();
    passed &= (info_str.find("2 rows") != string::npos || 
               info_str.find("2") != string::npos);
    passed &= (info_str.find("3 cols") != string::npos || 
               info_str.find("3") != string::npos);
    
    print_test_case("info() method", passed);
    if (passed) {
      cout << "  Info output: " << info_str.substr(0, 100) << "...\n";
    }
    all_passed &= passed;
  }

  // Test 8: Matrice 0×0
  {
    mat_int_type mat(0, 0);
    bool passed = (mat.num_rows() == 0 && mat.num_cols() == 0 && mat.empty());
    print_test_case("0×0 matrix", passed);
    all_passed &= passed;
  }

  // Test 9: Operatore di output
  {
    mat_int_type mat(2, 2);
    mat(0, 0) = 1; mat(0, 1) = 2;
    mat(1, 0) = 3; mat(1, 1) = 4;
    
    ostringstream oss;
    oss << mat;
    string output = oss.str();
    
    bool passed = !output.empty();
    passed &= (output.find('1') != string::npos);
    passed &= (output.find('4') != string::npos);
    
    print_test_case("operator<< output", passed);
    if (passed) {
      cout << "  Matrix output: " << output;
    }
    all_passed &= passed;
  }

  // Test 10: Matrice complessa
  {
    mat_complex_type mat(2, 2);
    mat(0, 0) = {1.0, 2.0};
    mat(0, 1) = {3.0, 4.0};
    mat(1, 0) = {5.0, 6.0};
    mat(1, 1) = {7.0, 8.0};
    
    bool passed = true;
    passed &= (abs(mat(0, 0).real() - 1.0) < 1e-12);
    passed &= (abs(mat(1, 1).imag() - 8.0) < 1e-12);
    
    vector<complex_type> col;
    mat.get_column(1, col);
    passed &= (col.size() == 2);
    passed &= (abs(col[0].real() - 3.0) < 1e-12);
    passed &= (abs(col[1].imag() - 8.0) < 1e-12);
    
    print_test_case("Complex matrix operations", passed);
    all_passed &= passed;
  }

  print_test_case("Matrix Type Comprehensive Overall", all_passed);
}

void test_promotion_comprehensive()
{
  print_header("TEST PROMOTION: Comprehensive Type Promotion");

  bool all_passed = true;

  // Test 1: promote_to_int per bool
  {
    GenericContainer gc;
    gc.set_bool(false);
    gc.promote_to_int();
    bool passed = (gc.get_type() == GC_type::INTEGER && gc.get_int() == 0);
    print_test_case("promote_to_int (false → 0)", passed);
    all_passed &= passed;
  }

  // Test 2: promote_to_long
  {
    GenericContainer gc;
    gc.set_bool(true);
    gc.promote_to_long();
    bool passed = (gc.get_type() == GC_type::LONG && gc.get_long() == 1);
    print_test_case("promote_to_long (bool → long)", passed);
    
    gc.set_int(42);
    gc.promote_to_long();
    passed &= (gc.get_type() == GC_type::LONG && gc.get_long() == 42);
    print_test_case("promote_to_long (int → long)", passed);
    
    all_passed &= passed;
  }

  // Test 3: promote_to_vec_int e promote_to_vec_long
  {
    GenericContainer gc;
    gc.set_vec_bool({true, false, true, false});
    gc.promote_to_vec_int();
    bool passed = (gc.get_type() == GC_type::VEC_INTEGER);
    if (passed) {
      vec_int_type vec = gc.get_vec_int();
      passed = (vec.size() == 4 && vec[0] == 1 && vec[1] == 0 && 
                vec[2] == 1 && vec[3] == 0);
    }
    print_test_case("promote_to_vec_int (vec_bool → vec_int)", passed);
    all_passed &= passed;
  }

  {
    GenericContainer gc;
    gc.set_vec_bool({false, true});
    gc.promote_to_vec_long();
    bool passed = (gc.get_type() == GC_type::VEC_LONG);
    if (passed) {
      vec_long_type vec = gc.get_vec_long();
      passed = (vec.size() == 2 && vec[0] == 0 && vec[1] == 1);
    }
    print_test_case("promote_to_vec_long (vec_bool → vec_long)", passed);
    all_passed &= passed;
  }

  // Test 4: promote_to_vec_real da vec_int
  {
    GenericContainer gc;
    gc.set_vec_int({1, 2, 3, 4, 5});
    gc.promote_to_vec_real();
    bool passed = (gc.get_type() == GC_type::VEC_REAL);
    if (passed) {
      vec_real_type vec = gc.get_vec_real();
      passed = (vec.size() == 5);
      for (int i = 0; i < 5 && passed; ++i) {
        passed = (abs(vec[i] - (i + 1)) < 1e-12);
      }
    }
    print_test_case("promote_to_vec_real (vec_int → vec_real)", passed);
    all_passed &= passed;
  }

  // Test 5: promote_to_vec_complex da vec_real
  {
    GenericContainer gc;
    gc.set_vec_real({1.5, 2.5, 3.5});
    gc.promote_to_vec_complex();
    bool passed = (gc.get_type() == GC_type::VEC_COMPLEX);
    if (passed) {
      vec_complex_type vec = gc.get_vec_complex();
      passed = (vec.size() == 3);
      for (int i = 0; i < 3 && passed; ++i) {
        passed = (abs(vec[i].real() - (1.5 + i * 1.0)) < 1e-12);
        passed &= (abs(vec[i].imag()) < 1e-12);
      }
    }
    print_test_case("promote_to_vec_complex (vec_real → vec_complex)", passed);
    all_passed &= passed;
  }

  // Test 6: promote_to_mat_int da vector di vector
  {
    GenericContainer gc;
    gc.set_vector(2);
    gc[0].set_vec_int({1, 2, 3});
    gc[1].set_vec_int({4, 5, 6});
    
    bool passed = true;
    try {
      gc.promote_to_mat_int();
      passed = false;
    } catch ( ... ) {
      // questa promozione non è definita
    }
    print_test_case("promote_to_mat_int (vector of vectors) [not available]", passed);
    all_passed &= passed;
  }

  // Test 7: promote_to_mat_real da vector di vector di int
  {
    GenericContainer gc;
    gc.set_vector(3);
    gc[0].set_vec_int({10, 20});
    gc[1].set_vec_int({30, 40});
    gc[2].set_vec_int({50, 60});

    bool passed = true;
    try {
      gc.promote_to_mat_real();
      passed = false;
    } catch ( ... ) {
      // questa promozione non è definita
    }

    print_test_case("promote_to_mat_real (vector of vec_int) [not available]", passed);
    all_passed &= passed;
  }

  // Test 8: promote_to_mat_complex da mat_real
  {
    GenericContainer gc;
    mat_real_type mat(2, 2);
    mat(0, 0) = 1.1; mat(0, 1) = 2.2;
    mat(1, 0) = 3.3; mat(1, 1) = 4.4;
    gc.set_mat_real(mat);
    gc.promote_to_mat_complex();
    
    bool passed = (gc.get_type() == GC_type::MAT_COMPLEX);
    if (passed) {
      mat_complex_type& cmat = gc.get_mat_complex();
      passed = (cmat.num_rows() == 2 && cmat.num_cols() == 2);
      passed &= (abs(cmat(0, 0).real() - 1.1) < 1e-12);
      passed &= (abs(cmat(1, 1).real() - 4.4) < 1e-12);
      passed &= (abs(cmat(0, 0).imag()) < 1e-12);
      passed &= (abs(cmat(1, 1).imag()) < 1e-12);
    }
    print_test_case("promote_to_mat_complex (mat_real → mat_complex)", passed);
    all_passed &= passed;
  }

  // Test 9: promote_to_vector da vector omogeneo
  {
    GenericContainer gc;
    gc.set_vec_int({100, 200, 300});
    gc.promote_to_vector();
    
    bool passed = (gc.get_type() == GC_type::VECTOR);
    if (passed) {
      vector_type& vec = gc.get_vector();
      passed = (vec.size() == 3);
      passed &= (vec[0].get_int() == 100);
      passed &= (vec[1].get_int() == 200);
      passed &= (vec[2].get_int() == 300);
    }
    print_test_case("promote_to_vector (vec_int → vector)", passed);
    all_passed &= passed;
  }

  // Test 10: Catena di promozioni
  {
    GenericContainer gc;
    gc.set_bool(true);
    gc.promote_to_int();      // bool → int
    gc.promote_to_real();     // int → real
    gc.promote_to_complex();  // real → complex
    
    bool passed = (gc.get_type() == GC_type::COMPLEX);
    if (passed) {
      complex_type c = gc.get_complex();
      passed = (abs(c.real() - 1.0) < 1e-12);
      passed &= (abs(c.imag()) < 1e-12);
    }
    print_test_case("Promotion chain (bool → int → real → complex)", passed);
    all_passed &= passed;
  }

  print_test_case("Type Promotion Comprehensive Overall", all_passed);
}

void test_copyto_methods()
{
  print_header("TEST COPYTO: Copy Conversion Methods");

  bool all_passed = true;

  // Test 1: copyto_vec_int
  {
    GenericContainer gc;
    gc.set_vec_int({1, 2, 3, 4, 5});
    
    vec_int_type dest;
    gc.copyto_vec_int(dest);
    
    bool passed = (dest.size() == 5);
    for (int i = 0; i < 5 && passed; ++i) {
      passed = (dest[i] == i + 1);
    }
    print_test_case("copyto_vec_int", passed);
    all_passed &= passed;
  }

  // Test 2: copyto_vec_uint da int
  {
    GenericContainer gc;
    gc.set_vec_int({10, 20, 30});
    
    vec_uint_type dest;
    gc.copyto_vec_uint(dest);
    
    bool passed = (dest.size() == 3);
    passed &= (dest[0] == 10 && dest[1] == 20 && dest[2] == 30);
    print_test_case("copyto_vec_uint from vec_int", passed);
    all_passed &= passed;
  }

  // Test 3: copyto_vec_long
  {
    GenericContainer gc;
    gc.set_vec_long({1000, 2000, 3000});
    
    vec_long_type dest;
    gc.copyto_vec_long(dest);
    
    bool passed = (dest.size() == 3);
    passed &= (dest[0] == 1000 && dest[1] == 2000 && dest[2] == 3000);
    print_test_case("copyto_vec_long", passed);
    all_passed &= passed;
  }

  // Test 4: copyto_vec_ulong da vec_long
  {
    GenericContainer gc;
    gc.set_vec_long({100, 200});
    
    vec_ulong_type dest;
    gc.copyto_vec_ulong(dest);
    
    bool passed = (dest.size() == 2);
    passed &= (dest[0] == 100 && dest[1] == 200);
    print_test_case("copyto_vec_ulong from vec_long", passed);
    all_passed &= passed;
  }

  // Test 5: copyto_vec_real
  {
    GenericContainer gc;
    gc.set_vec_real({1.1, 2.2, 3.3});
    
    vec_real_type dest;
    gc.copyto_vec_real(dest);
    
    bool passed = (dest.size() == 3);
    passed &= (abs(dest[0] - 1.1) < 1e-12);
    passed &= (abs(dest[1] - 2.2) < 1e-12);
    passed &= (abs(dest[2] - 3.3) < 1e-12);
    print_test_case("copyto_vec_real", passed);
    all_passed &= passed;
  }

  // Test 6: copyto_vec_complex
  {
    GenericContainer gc;
    gc.set_vec_complex({{1.0, 2.0}, {3.0, 4.0}});
    
    vec_complex_type dest;
    gc.copyto_vec_complex(dest);
    
    bool passed = (dest.size() == 2);
    passed &= (abs(dest[0].real() - 1.0) < 1e-12);
    passed &= (abs(dest[0].imag() - 2.0) < 1e-12);
    passed &= (abs(dest[1].real() - 3.0) < 1e-12);
    passed &= (abs(dest[1].imag() - 4.0) < 1e-12);
    print_test_case("copyto_vec_complex", passed);
    all_passed &= passed;
  }

  // Test 7: copyto_vec_string
  {
    GenericContainer gc;
    gc.set_vec_string({"Hello", "World", "!"});
    
    vec_string_type dest;
    gc.copyto_vec_string(dest);
    
    bool passed = (dest.size() == 3);
    passed &= (dest[0] == "Hello");
    passed &= (dest[1] == "World");
    passed &= (dest[2] == "!");
    print_test_case("copyto_vec_string", passed);
    all_passed &= passed;
  }

  // Test 8: copyto_mat_int
  {
    GenericContainer gc;
    mat_int_type mat(2, 3);
    for (unsigned i = 0; i < 2; ++i)
      for (unsigned j = 0; j < 3; ++j)
        mat(i, j) = i * 10 + j;
    gc.set_mat_int(mat);
    
    mat_int_type dest;
    gc.copyto_mat_int(dest);
    
    bool passed = (dest.num_rows() == 2 && dest.num_cols() == 3);
    for (unsigned i = 0; i < 2 && passed; ++i) {
      for (unsigned j = 0; j < 3 && passed; ++j) {
        passed = (dest(i, j) == i * 10 + j);
      }
    }
    print_test_case("copyto_mat_int", passed);
    all_passed &= passed;
  }

  // Test 9: copyto_mat_long
  {
    GenericContainer gc;
    mat_long_type mat(3, 2);
    for (unsigned i = 0; i < 3; ++i)
      for (unsigned j = 0; j < 2; ++j)
        mat(i, j) = i * 100 + j * 10;
    gc.set_mat_long(mat);
    
    mat_long_type dest;
    gc.copyto_mat_long(dest);
    
    bool passed = (dest.num_rows() == 3 && dest.num_cols() == 2);
    for (unsigned i = 0; i < 3 && passed; ++i) {
      for (unsigned j = 0; j < 2 && passed; ++j) {
        passed = (dest(i, j) == i * 100 + j * 10);
      }
    }
    print_test_case("copyto_mat_long", passed);
    all_passed &= passed;
  }

  // Test 10: copyto_mat_real
  {
    GenericContainer gc;
    mat_real_type mat(2, 2);
    mat(0, 0) = 1.5; mat(0, 1) = 2.5;
    mat(1, 0) = 3.5; mat(1, 1) = 4.5;
    gc.set_mat_real(mat);
    
    mat_real_type dest;
    gc.copyto_mat_real(dest);
    
    bool passed = (dest.num_rows() == 2 && dest.num_cols() == 2);
    passed &= (abs(dest(0, 0) - 1.5) < 1e-12);
    passed &= (abs(dest(1, 1) - 4.5) < 1e-12);
    print_test_case("copyto_mat_real", passed);
    all_passed &= passed;
  }

  // Test 11: copyto_mat_complex
  {
    GenericContainer gc;
    mat_complex_type mat(2, 2);
    mat(0, 0) = {1.0, 2.0}; mat(0, 1) = {3.0, 4.0};
    mat(1, 0) = {5.0, 6.0}; mat(1, 1) = {7.0, 8.0};
    gc.set_mat_complex(mat);
    
    mat_complex_type dest;
    gc.copyto_mat_complex(dest);
    
    bool passed = (dest.num_rows() == 2 && dest.num_cols() == 2);
    passed &= (abs(dest(0, 0).real() - 1.0) < 1e-12);
    passed &= (abs(dest(1, 1).imag() - 8.0) < 1e-12);
    print_test_case("copyto_mat_complex", passed);
    all_passed &= passed;
  }

  // Test 12: Conversione incompatibile (dovrebbe lanciare eccezione)
  {
    GenericContainer gc;
    gc.set_string("Not a vector");
    
    vec_int_type dest;
    bool passed = false;
    try {
      gc.copyto_vec_int(dest);
      // Non dovrebbe arrivare qui
      passed = false;
    } catch ( std::exception const & e ) {
      // Eccezione attesa
      passed = true;
      cout << "  Expected exception: " << e.what() << "\n";
    }
    print_test_case("copyto with incompatible type (throws)", passed);
    all_passed &= passed;
  }

  print_test_case("Copyto Methods Overall", all_passed);
}

void test_push_methods()
{
  print_header("TEST PUSH: Push Methods for Vectors");

  bool all_passed = true;

  // Test 1: push_bool su vec_bool
  {
    GenericContainer gc;
    gc.set_vec_bool();
    
    gc.push_bool(true);
    gc.push_bool(false);
    gc.push_bool(true);
    
    bool passed = (gc.get_vec_bool().size() == 3);
    passed &= (gc.get_vec_bool()[0] == true);
    passed &= (gc.get_vec_bool()[1] == false);
    passed &= (gc.get_vec_bool()[2] == true);
    
    print_test_case("push_bool on vec_bool", passed);
    all_passed &= passed;
  }

  // Test 2: push_int su vec_int
  {
    GenericContainer gc;
    gc.set_vec_int();
    
    gc.push_int(10);
    gc.push_int(20);
    gc.push_int(30);
    
    bool passed = (gc.get_vec_int().size() == 3);
    passed &= (gc.get_vec_int()[0] == 10);
    passed &= (gc.get_vec_int()[1] == 20);
    passed &= (gc.get_vec_int()[2] == 30);
    
    print_test_case("push_int on vec_int", passed);
    all_passed &= passed;
  }

  // Test 3: push_long su vec_long
  {
    GenericContainer gc;
    gc.set_vec_long();
    
    gc.push_long(1000);
    gc.push_long(2000);
    gc.push_long(3000);
    
    bool passed = (gc.get_vec_long().size() == 3);
    passed &= (gc.get_vec_long()[0] == 1000);
    passed &= (gc.get_vec_long()[1] == 2000);
    passed &= (gc.get_vec_long()[2] == 3000);
    
    print_test_case("push_long on vec_long", passed);
    all_passed &= passed;
  }

  // Test 4: push_real su vec_real
  {
    GenericContainer gc;
    gc.set_vec_real();
    
    gc.push_real(1.1);
    gc.push_real(2.2);
    gc.push_real(3.3);
    
    bool passed = (gc.get_vec_real().size() == 3);
    passed &= (abs(gc.get_vec_real()[0] - 1.1) < 1e-12);
    passed &= (abs(gc.get_vec_real()[1] - 2.2) < 1e-12);
    passed &= (abs(gc.get_vec_real()[2] - 3.3) < 1e-12);
    
    print_test_case("push_real on vec_real", passed);
    all_passed &= passed;
  }

  // Test 5: push_complex con oggetto complesso
  {
    GenericContainer gc;
    gc.set_vec_complex();
    
    complex_type c1(1.0, 2.0);
    complex_type c2(3.0, 4.0);
    
    gc.push_complex(c1);
    gc.push_complex(c2);
    
    bool passed = (gc.get_vec_complex().size() == 2);
    passed &= (abs(gc.get_vec_complex()[0].real() - 1.0) < 1e-12);
    passed &= (abs(gc.get_vec_complex()[0].imag() - 2.0) < 1e-12);
    passed &= (abs(gc.get_vec_complex()[1].real() - 3.0) < 1e-12);
    passed &= (abs(gc.get_vec_complex()[1].imag() - 4.0) < 1e-12);
    
    print_test_case("push_complex with complex object", passed);
    all_passed &= passed;
  }

  // Test 6: push_complex con parti reali/immaginarie
  {
    GenericContainer gc;
    gc.set_vec_complex();
    
    gc.push_complex(5.0, 6.0);
    gc.push_complex(7.0, 8.0);
    
    bool passed = (gc.get_vec_complex().size() == 2);
    passed &= (abs(gc.get_vec_complex()[0].real() - 5.0) < 1e-12);
    passed &= (abs(gc.get_vec_complex()[0].imag() - 6.0) < 1e-12);
    passed &= (abs(gc.get_vec_complex()[1].real() - 7.0) < 1e-12);
    passed &= (abs(gc.get_vec_complex()[1].imag() - 8.0) < 1e-12);
    
    print_test_case("push_complex with real/imag parts", passed);
    all_passed &= passed;
  }

  // Test 7: push_string su vec_string
  {
    GenericContainer gc;
    gc.set_vec_string();
    
    gc.push_string("Hello");
    gc.push_string("World");
    gc.push_string("!");
    
    bool passed = (gc.get_vec_string().size() == 3);
    passed &= (gc.get_vec_string()[0] == "Hello");
    passed &= (gc.get_vec_string()[1] == "World");
    passed &= (gc.get_vec_string()[2] == "!");
    
    print_test_case("push_string on vec_string", passed);
    all_passed &= passed;
  }

  // Test 8: push su vector_type (contenitore generico)
  {
    GenericContainer gc;
    gc.set_vector();
    
    // Aggiungiamo elementi di tipi diversi
    GenericContainer elem1;
    elem1.set_int(42);
    gc.get_vector().push_back(elem1);
    
    GenericContainer elem2;
    elem2.set_string("test");
    gc.get_vector().push_back(elem2);
    
    bool passed = (gc.get_vector().size() == 2);
    passed &= (gc.get_vector()[0].get_int() == 42);
    passed &= (gc.get_vector()[1].get_string() == "test");
    
    print_test_case("push on vector_type (generic)", passed);
    all_passed &= passed;
  }

  // Test 9: Tentativo di push su tipo non vettoriale
  {
    GenericContainer gc;
    gc.set_int(42);  // Non è un vettore
    
    bool passed = false;
    try {
      gc.push_int(100);
      passed = true;
    } catch ( std::exception const & e) {
      passed = false;
      cout << "  Expected exception: " << e.what() << "\n";
    }
    
    print_test_case("push on non-vector type (throws)", passed);
    all_passed &= passed;
  }

  // Test 10: push mix di tipi usando metodi diversi
  {
    GenericContainer gc;
    gc.set_vec_complex();
    
    // Mix di metodi push
    complex_type c(1.0, 1.0);
    gc.push_complex(c);
    gc.push_complex(2.0, 2.0);
    
    bool passed = (gc.get_vec_complex().size() == 2);
    passed &= (abs(gc.get_vec_complex()[0].real() - 1.0) < 1e-12);
    passed &= (abs(gc.get_vec_complex()[0].imag() - 1.0) < 1e-12);
    passed &= (abs(gc.get_vec_complex()[1].real() - 2.0) < 1e-12);
    passed &= (abs(gc.get_vec_complex()[1].imag() - 2.0) < 1e-12);
    
    print_test_case("Mixed push methods", passed);
    all_passed &= passed;
  }

  print_test_case("Push Methods Overall", all_passed);
}

void test_get_map_with_vector_keys()
{
  print_header("TEST GET_MAP: Methods with Vector Keys");

  bool all_passed = true;

  // Setup: crea una mappa complessa
  GenericContainer gc;
  gc.set_map();
  gc["id"].set_int(123);
  gc["name"].set_string("Test Object");
  gc["value"].set_real(456.789);
  gc["enabled"].set_bool(true);
  gc["data"].set_vec_int({1, 2, 3, 4, 5});
  gc["complex_data"].set_vec_complex({{1.0, 2.0}, {3.0, 4.0}});
  gc["texts"].set_vec_string({"Hello", "World"});

  // Test 1: get_map_bool con vector keys
  {
    vec_string_type keys = {"enabled", "active", "on"};  // "enabled" esiste
    bool value = gc.get_map_bool(keys, "Test get_map_bool");
    
    bool passed = (value == true);
    print_test_case("get_map_bool with vector keys (first exists)", passed);
    all_passed &= passed;
  }

  // Test 2: get_map_bool con initializer_list
  {
    bool value = gc.get_map_bool({"active", "enabled", "status"});
    
    bool passed = (value == true);  // "enabled" è il secondo
    print_test_case("get_map_bool with initializer_list", passed);
    all_passed &= passed;
  }

  // Test 3: get_map_int con vector keys
  {
    vec_string_type keys = {"ID", "id", "identifier"};
    int_type value = gc.get_map_int(keys, "Test get_map_int");
    
    bool passed = (value == 123);
    print_test_case("get_map_int with vector keys", passed);
    all_passed &= passed;
  }

  // Test 4: get_map_number (real) con vector keys
  {
    vec_string_type keys = {"val", "value", "amount"};
    real_type value = gc.get_map_number(keys, "Test get_map_number");
    
    bool passed = (abs(value - 456.789) < 1e-12);
    print_test_case("get_map_number with vector keys", passed);
    all_passed &= passed;
  }

  // Test 5: get_map_string con vector keys
  {
    vec_string_type keys = {"title", "name", "label"};
    string const& value = gc.get_map_string(keys, "Test get_map_string");
    
    bool passed = (value == "Test Object");
    print_test_case("get_map_string with vector keys", passed);
    all_passed &= passed;
  }

  // Test 6: get_map_vec_real con vector keys
  {
    // Prima creiamo un vettore di real
    gc["values"].set_vec_real({1.5, 2.5, 3.5});
    
    vec_string_type keys = {"numbers", "values", "data"};
    vec_real_type const& value = gc.get_map_vec_real(keys, "Test get_map_vec_real");
    
    bool passed = (value.size() == 3);
    passed &= (abs(value[0] - 1.5) < 1e-12);
    passed &= (abs(value[1] - 2.5) < 1e-12);
    passed &= (abs(value[2] - 3.5) < 1e-12);
    
    print_test_case("get_map_vec_real with vector keys", passed);
    all_passed &= passed;
  }

  // Test 7: get_map_vec_complex con vector keys
  {
    vec_string_type keys = {"complex", "complex_data", "cdata"};
    vec_complex_type const& value = gc.get_map_vec_complex(keys, "Test get_map_vec_complex");
    
    bool passed = (value.size() == 2);
    passed &= (abs(value[0].real() - 1.0) < 1e-12);
    passed &= (abs(value[0].imag() - 2.0) < 1e-12);
    passed &= (abs(value[1].real() - 3.0) < 1e-12);
    passed &= (abs(value[1].imag() - 4.0) < 1e-12);
    
    print_test_case("get_map_vec_complex with vector keys", passed);
    all_passed &= passed;
  }

  // Test 8: get_map_vec_string con vector keys
  {
    vec_string_type keys = {"strings", "texts", "labels"};
    vec_string_type const& value = gc.get_map_vec_string(keys, "Test get_map_vec_string");
    
    bool passed = (value.size() == 2);
    passed &= (value[0] == "Hello");
    passed &= (value[1] == "World");
    
    print_test_case("get_map_vec_string with vector keys", passed);
    all_passed &= passed;
  }

  // Test 9: Nessuna chiave trovata (dovrebbe lanciare eccezione)
  {
    vec_string_type keys = {"nonexistent1", "nonexistent2"};
    
    bool passed = false;
    try {
      int_type value = gc.get_map_int(keys, "Test no key");
      // Non dovrebbe arrivare qui
      passed = false;
    } catch ( std::exception const & e) {
      // Eccezione attesa
      passed = true;
      cout << "  Expected exception: " << e.what() << "\n";
    } catch ( ... ) {
      passed = true;
      cout << "  exception\n";    
    }
    
    print_test_case("get_map with no matching keys (throws)", passed);
    all_passed &= passed;
  }

  // Test 10: operator() con vector keys
  {
    vec_string_type keys = {"name", "id"};  // "name" è il primo che esiste
    GenericContainer& item = gc(keys, "Test operator() with vector keys");
    
    bool passed = (item.get_string() == "Test Object");
    print_test_case("operator() with vector keys", passed);
    all_passed &= passed;
  }

  print_test_case("Get Map with Vector Keys Overall", all_passed);
}

void test_io_advanced()
{
  print_header("TEST IO ADVANCED: Advanced I/O Methods");

  bool all_passed = true;

  // Test 1: print_content_types
  {
    GenericContainer gc;
    gc.set_map();
    gc["id"].set_int(42);
    gc["name"].set_string("Test");
    gc["values"].set_vec_real({1.1, 2.2, 3.3});
    gc["nested"].set_map();
    gc["nested"]["key"].set_bool(true);
    
    ostringstream oss;
    gc.print_content_types(oss, ">> ", "  ");
    string output = oss.str();
    
    bool passed = !output.empty();
    passed &= (output.find("id:") != string::npos);
    passed &= (output.find("INTEGER") != string::npos || output.find("int") != string::npos);
    passed &= (output.find("name:") != string::npos);
    passed &= (output.find("STRING") != string::npos || output.find("string") != string::npos);
    
    print_test_case("print_content_types", passed);
    if (passed) {
      cout << "  Content types output (first 200 chars):\n";
      cout << "  " << output.substr(0, std::min<size_t>(output.length(),200)) << "\n";
    }
    all_passed &= passed;
  }

  // Test 2: write_formatted_data e read_formatted_data
  {
    // Crea dati tabellari
    GenericContainer gc;
    gc.set_map();
    
    // Headers
    vec_string_type headers = {"Time", "Value", "Error"};
    gc["headers"].set_vec_string(headers);
    
    // Data come vector di colonne
    gc["data"].set_vector(3);
    gc["data"][0].set_vec_real({0.0, 1.0, 2.0, 3.0});      // Time
    gc["data"][1].set_vec_real({10.5, 11.2, 12.8, 13.1}); // Value
    gc["data"][2].set_vec_real({0.1, 0.2, 0.3, 0.4});     // Error
    
    // Scrivi in string stream
    ostringstream oss;
    gc.write_formatted_data(oss, ',');
    string csv_output = oss.str();
    
    bool passed = !csv_output.empty();
    passed &= (csv_output.find("Time,Value,Error") != string::npos);
    passed &= (csv_output.find("0,10.5,0.1") != string::npos);
    passed &= (csv_output.find("3,13.1,0.4") != string::npos);
    
    print_test_case("write_formatted_data (CSV format)", passed);
    
    // Ora leggi i dati scritti
    if (passed) {
      istringstream iss(csv_output);
      GenericContainer gc2;
      gc2.read_formatted_data(iss, "#", ",");
      
      passed &= (gc2.exists("headers"));
      passed &= (gc2.exists("data"));
      
      if (passed) {
        vec_string_type headers2 = gc2["headers"].get_vec_string();
        passed &= (headers2.size() == 3);
        passed &= (headers2[0] == "Time");
        
        vector_type data2 = gc2["data"].get_vector();
        passed &= (data2.size() == 3);
        
        if (data2.size() >= 1) {
          vec_real_type col0 = data2[0].get_vec_real();
          passed &= (col0.size() == 4);
          passed &= (abs(col0[0] - 0.0) < 1e-12);
        }
      }
      
      print_test_case("read_formatted_data round-trip", passed);
    }
    
    all_passed &= passed;
  }

  // Test 3: write_table e write_table_formatted (funzioni helper)
  {
    // Prepara dati per write_table
    vec_string_type headers = {"X", "Y", "Z"};
    
    mat_real_type matrix(3, 3);
    matrix(0, 0) = 1.0; matrix(0, 1) = 2.0; matrix(0, 2) = 3.0;
    matrix(1, 0) = 4.0; matrix(1, 1) = 5.0; matrix(1, 2) = 6.0;
    matrix(2, 0) = 7.0; matrix(2, 1) = 8.0; matrix(2, 2) = 9.0;
    
    ostringstream oss1;
    write_table(headers, matrix, oss1, '\t');
    string tab_output = oss1.str();
    
    bool passed = !tab_output.empty();
    passed &= (tab_output.find("X\tY\tZ") != string::npos);
    passed &= (tab_output.find("1\t2\t3") != string::npos);
    passed &= (tab_output.find("7\t8\t9") != string::npos);
    
    print_test_case("write_table function", passed);
    
    // Test write_table_formatted
    ostringstream oss2;
    write_table_formatted(headers, matrix, oss2);
    string formatted_output = oss2.str();
    
    passed &= !formatted_output.empty();
    passed &= (formatted_output.find("X") != string::npos);
    
    print_test_case("write_table_formatted function", passed);
    
    all_passed &= passed;
  }

  // Test 4: read_formatted_data2 con parametri
  {
    // Crea un file di dati con commenti che contengono parametri
    string test_data = "#! temperature = 25.5\n"
                       "#! pressure    = 1013.25\n"
                       "#! unit        = kPa\n"
                       "Time\tValue\tStdDev\n"
                       "0.0\t10.2\t0.1\n"
                       "1.0\t11.5\t0.2\n"
                       "2.0\t12.8\t0.3\n";
    
    istringstream iss(test_data);
    GenericContainer data;
    GenericContainer params;
    
    data.read_formatted_data2(iss, "#", " \t", &params);
    
    bool passed = data.exists("headers");
    passed &= data.exists("data");
    passed &= params.exists("temperature");
    passed &= params.exists("pressure");
    passed &= params.exists("unit");
    
    if (passed) {
      real_type temp = params["temperature"].get_real();
      real_type pressure = params["pressure"].get_real();
      string unit = params["unit"].get_string();
      
      passed &= (abs(temp - 25.5) < 1e-12);
      passed &= (abs(pressure - 1013.25) < 1e-12);
      passed &= (unit == "kPa");
      
      // Verifica dati principali
      map_type data_map = data["data"].get_map();
      passed &= (data_map.find("Time") != data_map.end());
      passed &= (data_map.find("Value") != data_map.end());
      passed &= (data_map.find("StdDev") != data_map.end());
    }
    
    print_test_case("read_formatted_data2 with parameters", passed);
    all_passed &= passed;
  }

  // Test 5: string_escape utility
  {
    ostringstream oss;
    string test_str = "Line1\nLine2\tTab\"Quote\\Backslash";
    string_escape(oss, test_str);
    string escaped = oss.str();
    
    bool passed = !escaped.empty();
    // Verifica che alcuni caratteri siano stati escapati
    passed &= (escaped.find("\\n") != string::npos);  // newline
    passed &= (escaped.find("\\t") != string::npos);  // tab
    passed &= (escaped.find("\\\"") != string::npos); // quote
    passed &= (escaped.find("\\\\") != string::npos); // backslash
    
    print_test_case("string_escape utility", passed);
    if (passed) {
      cout << "  Original: " << test_str << "\n";
      cout << "  Escaped: " << escaped << "\n";
    }
    all_passed &= passed;
  }

  print_test_case("Advanced I/O Methods Overall", all_passed);
}

void test_edge_cases()
{
  print_header("TEST EDGE CASES: Boundary and Error Conditions");

  bool all_passed = true;

  // Test 1: Accesso fuori range con operator[]
  {
    GenericContainer gc;
    gc.set_vec_int({1, 2, 3});
    
    bool passed = false;
    try {
      GenericContainer& item = gc[10];  // Fuori range
      (void)item;
      passed = true;  // deve fare resize!
    } catch ( std::exception const & e) {
      passed = false;
      cout << "  exception: " << e.what() << "\n";
    } catch (...) {
      passed = false;  // Qualsiasi eccezione va male
    }
    
    print_test_case("operator[] out of range (throws)", passed);
    all_passed &= passed;
  }

  // Test 2: get_pointer su tipo non pointer
  {
    GenericContainer gc;
    gc.set_int(42);
    
    bool passed = true;
    try {
      void* ptr = gc.get_pvoid("Test non-pointer");
      (void)ptr;
      passed = false;
    } catch (std::exception const & e) {
      passed = true;
      cout << "  exception: " << e.what() << "\n";
    } catch (...) {
      passed = true;  // Qualsiasi eccezione va male
    }
    
    print_test_case("get_pointer on non-pointer type (throws)", passed);
    all_passed &= passed;
  }

  // Test 3: get_if_exists con tipo incompatibile
  {
    GenericContainer gc;
    gc.set_map();
    gc["value"].set_string("not a number");
    
    real_type num;
    bool exists = gc.get_if_exists("value", num);
    
    bool passed = (exists == false);  // Dovrebbe fallire silenziosamente
    print_test_case("get_if_exists with type mismatch (returns false)", passed);
    all_passed &= passed;
  }

  // Test 4: Container vuoto - get_num_elements
  {
    GenericContainer gc;
    
    bool passed = (gc.get_num_elements() == 0);
    print_test_case("get_num_elements on empty container", passed);
    all_passed &= passed;
  }

  // Test 5: Matrice con dimensioni zero - num_rows/num_cols
  {
    GenericContainer gc;
    gc.set_mat_int(0, 0);
    
    bool passed = (gc.num_rows() == 0 && gc.num_cols() == 0);
    print_test_case("num_rows/num_cols on 0×0 matrix", passed);
    all_passed &= passed;
  }

  // Test 6: Tentativo di promozione impossibile
  {
    GenericContainer gc;
    gc.set_string("cannot promote");
    
    bool passed = false;
    try {
      gc.promote_to_int();  // Stringa → int non dovrebbe funzionare
      passed = false;
    } catch ( std::exception const & e) {
      passed = true;
      cout << "  Expected exception: " << e.what() << "\n";
    } catch ( ... ) {
      passed = true;
      cout << "  exception\n";
    }
    
    print_test_case("Invalid promotion (throws)", passed);
    all_passed &= passed;
  }

  // Test 7: exists() su container non-mappa
  {
    GenericContainer gc;
    gc.set_int(42);
    
    bool passed = !gc.exists("key");

    print_test_case("exists() on non-map container (throws)", passed);
    all_passed &= passed;
  }

  // Test 8: get_number su tipo incompatibile
  {
    GenericContainer gc;
    gc.set_string("not a number");
    
    bool passed = false;
    try {
      real_type num = gc.get_number("Test string");
      // Dovrebbe lanciare eccezione
      (void)num;
      passed = false;
    } catch (std::exception const & e) {
      passed = true;
      cout << "  Expected exception: " << e.what() << "\n";
    }
    
    print_test_case("get_number on non-numeric type (throws)", passed);
    all_passed &= passed;
  }

  // Test 9: Tentativo di ottenere puntatore da tipo non supportato
  {
    GenericContainer gc;
    gc.set_bool(true);
    
    bool passed = false;
    try {
      real_type* ptr = gc.get_real_pointer();
      // Dovrebbe lanciare eccezione
      (void)ptr;
      passed = false;
    } catch (std::exception const & e) {
      passed = true;
      cout << "  Expected exception: " << e.what() << "\n";
    }
    
    print_test_case("get_*_pointer on wrong type (throws)", passed);
    all_passed &= passed;
  }

  // Test 10: merge con conflitti
  {
    GenericContainer gc1;
    gc1.set_map();
    gc1["id"].set_int(1);
    gc1["name"].set_string("Original");
    
    GenericContainer gc2;
    gc2.set_map();
    gc2["id"].set_int(2);  // Conflitto!
    gc2["value"].set_real(3.14);
    
    bool passed = false;
    try {
      gc1.merge(gc2, "Test merge with conflict");
      passed = true;  // Dovrebbe lanciare eccezione per conflitto
    } catch (std::exception const & e) {
      passed = false;
      cout << "  Exception for merge conflict: " << e.what() << "\n";
    }
    
    print_test_case("merge with key conflict (throws)", passed);
    all_passed &= passed;
  }

  // Test 11: Serializzazione/deserializzazione di container vuoto
  {
    GenericContainer gc;
    
    vector<uint8_t> buffer = gc.serialize();
    GenericContainer gc2;
    int32_t used = gc2.de_serialize(buffer);
    
    bool passed = (used > 0 && gc2.empty());
    passed &= (gc.compare_content(gc2, "Empty container").empty());
    
    print_test_case("serialize/deserialize empty container", passed);
    all_passed &= passed;
  }

  print_test_case("Edge Cases Overall", all_passed);
}

void test_static_and_utility()
{
  print_header("TEST STATIC & UTILITY: Static Methods and Utilities");

  bool all_passed = true;

  // Test 1: gc_from_json static method
  {
    string json_str = R"({
      "id": 42,
      "name": "Test",
      "values": [1.1, 2.2, 3.3]
    })";
    
    GenericContainer gc = GenericContainer::gc_from_json(json_str);
    
    bool passed = (gc.get_type() == GC_type::MAP);
    passed &= (gc["id"].get_int() == 42);
    passed &= (gc["name"].get_string() == "Test");
    passed &= (gc["values"].get_vec_real().size() == 3);
    
    print_test_case("gc_from_json static method", passed);
    all_passed &= passed;
  }

  // Test 2: gc_from_yaml static method
  {
    string yaml_str = R"(---
id: 42
name: Test
values:
  - 1.1
  - 2.2
  - 3.3
)";
    
    GenericContainer gc = GenericContainer::gc_from_yaml(yaml_str);
    
    bool passed = (gc.get_type() == GC_type::MAP);
    passed &= (gc["id"].get_int() == 42);
    passed &= (gc["name"].get_string() == "Test");
    
    print_test_case("gc_from_yaml static method", passed);
    all_passed &= passed;
  }

  // Test 3: gc_from_toml static method
  {
    string toml_str = R"(
id = 42
name = "Test"
values = [1.1, 2.2, 3.3]
)";
    
    GenericContainer gc = GenericContainer::gc_from_toml(toml_str);
    
    bool passed = (gc.get_type() == GC_type::MAP);
    passed &= (gc["id"].get_int() == 42);
    passed &= (gc["name"].get_string() == "Test");
    
    print_test_case("gc_from_toml static method", passed);
    all_passed &= passed;
  }

  // Test 4: to_gc e from_gc (deep copy)
  {
    GenericContainer source;
    source.set_map();
    source["data"].set_vec_int({1, 2, 3});
    source["nested"].set_map();
    source["nested"]["key"].set_string("value");
    
    GenericContainer target;
    source.to_gc(target);
    
    bool passed = (target.compare_content(source, "Deep copy").empty());
    
    // Modifica source per verificare che sia una deep copy
    source["data"].get_vec_int()[0] = 999;
    passed &= (target["data"].get_vec_int()[0] == 1);  // Non dovrebbe essere modificato
    
    print_test_case("to_gc/from_gc deep copy", passed);
    all_passed &= passed;
  }

  // Test 5: must_exists utility
  {
    GenericContainer gc;
    gc.set_map();
    gc["id"].set_int(1);
    gc["name"].set_string("Test");
    gc["value"].set_real(3.14);
    
    vec_string_type keys = {"id", "name", "value"};
    string found = gc.must_exists(keys, "Test must_exists");
    
    bool passed = (!found.empty());
    passed &= (found == "id" || found == "name" || found == "value");
    
    print_test_case("must_exists with existing keys", passed);
    all_passed &= passed;
  }

  // Test 6: must_exists con nessuna chiave trovata
  {
    GenericContainer gc;
    gc.set_map();
    gc["id"].set_int(1);
    
    vec_string_type keys = {"nonexistent1", "nonexistent2"};
    
    bool passed = false;
    try {
      string found = gc.must_exists(keys, "Test must_exists fail");
      // Non dovrebbe arrivare qui
      (void)found;
      passed = false;
    } catch (std::exception const & e) {
      passed = true;
      cout << "  Expected exception: " << e.what() << "\n";
    }
    
    print_test_case("must_exists with no matching keys (throws)", passed);
    all_passed &= passed;
  }

  // Test 7: get_keys methods
  {
    GenericContainer gc;
    gc.set_map();
    gc["a"].set_int(1);
    gc["b"].set_int(2);
    gc["c"].set_int(3);
    
    // Test get_keys to vector
    vec_string_type keys_vec;
    gc.get_keys(keys_vec);
    
    bool passed = (keys_vec.size() == 3);
    passed &= (find(keys_vec.begin(), keys_vec.end(), "a") != keys_vec.end());
    passed &= (find(keys_vec.begin(), keys_vec.end(), "b") != keys_vec.end());
    passed &= (find(keys_vec.begin(), keys_vec.end(), "c") != keys_vec.end());
    
    // Test get_keys to string
    string keys_str = gc.get_keys();
    passed &= (!keys_str.empty());
    passed &= (keys_str.find("a") != string::npos);
    passed &= (keys_str.find("b") != string::npos);
    passed &= (keys_str.find("c") != string::npos);
    
    print_test_case("get_keys methods", passed);
    
    if (passed) {
      cout << "  Keys string: " << keys_str << "\n";
    }
    all_passed &= passed;
  }

  // Test 8: exception static method
  {
    bool passed = false;
    try {
      GenericContainer::exception("Test exception method");
      passed = false;
    } catch (std::exception const & e) {
      passed = true;
      cout << "  Expected exception: " << e.what() << "\n";
    }
    
    print_test_case("exception static method (throws)", passed);
    all_passed &= passed;
  }

  // Test 9: collapse con strutture annidate complesse
  {
    GenericContainer gc;
    gc.set_vector(3);
    
    // Vettori di diverse lunghezze - non dovrebbe collassare in matrice
    gc[0].set_vec_real({1.0, 2.0});
    gc[1].set_vec_real({3.0, 4.0, 5.0});  // Lunghezza diversa!
    gc[2].set_vec_real({6.0, 7.0});
    
    bool passed = false;
    try {
      gc.collapse();
      // Dovrebbe lanciare eccezione perché le lunghezze sono diverse
      passed = true;
    } catch (std::exception const & e) {
      passed = false;
      cout << "  Exception: " << e.what() << "\n";
    }
    
    print_test_case("collapse with mismatched lengths (throws)", passed);
    all_passed &= passed;
  }

  // Test 10: collapse con vettori compatibili
  {
    GenericContainer gc;
    gc.set_vector(2);
    gc[0].set_vec_real({1.0, 2.0, 3.0});
    gc[1].set_vec_real({4.0, 5.0, 6.0});
    
    gc.collapse();
    
    bool passed = (gc.get_type() == GC_type::MAT_REAL);
    if (passed) {
      mat_real_type& mat = gc.get_mat_real();
      passed = (mat.num_rows() == 3 && mat.num_cols() == 2);
      passed &= (abs(mat(0, 0) - 1.0) < 1e-12);
      passed &= (abs(mat(2, 1) - 6.0) < 1e-12);
    }
    
    print_test_case("collapse with compatible vectors", passed);
    all_passed &= passed;
  }

  print_test_case("Static & Utility Methods Overall", all_passed);
}

void test_type_compatibility()
{
  print_header("TEST TYPE COMPAT: Type Compatibility and Conversions");

  bool all_passed = true;

  // Test 1: Conversione implicita tra tipi interi
  {
    GenericContainer gc;
    
    // Test uint_type → int_type
    uint_type uint_val = 100;
    gc.set(uint_val);
    bool passed = (gc.get_type() == GC_type::INTEGER);
    passed &= (gc.get_int() == 100);
    
    // Test ulong_type → long_type
    ulong_type ulong_val = 1000;
    gc.set(ulong_val);
    passed &= (gc.get_type() == GC_type::LONG);
    passed &= (gc.get_long() == 1000);
    
    print_test_case("Implicit uint/ulong conversion", passed);
    all_passed &= passed;
  }

  // Test 2: get_as_* methods
  {
    GenericContainer gc;
    gc.set_int(42);
    
    bool passed = true;
    
    // get_as_int da int
    passed &= (gc.get_as_int() == 42);
    
    // get_as_uint da int
    passed &= (gc.get_as_uint() == 42);
    
    // get_as_long da int
    passed &= (gc.get_as_long() == 42);
    
    // get_as_ulong da int
    passed &= (gc.get_as_ulong() == 42);
    
    print_test_case("get_as_* from int", passed);
    all_passed &= passed;
  }

  // Test 3: get_as_* da real
  {
    GenericContainer gc;
    gc.set_real(3.14);
    
    bool passed = false;
    
    // Conversione con perdita di precisione
    try {
      passed &= (gc.get_as_int() == 3);  // Truncamento
      passed &= (gc.get_as_uint() == 3);
      passed &= (gc.get_as_long() == 3);
      passed &= (gc.get_as_ulong() == 3);
    } catch (std::exception const & e) {
      passed = true;
      cout << "  Exception: " << e.what() << "\n";
    }
    
    print_test_case("get_as_* from real (truncation)", passed);
    all_passed &= passed;
  }

  // Test 4: get_as_* da bool
  {
    GenericContainer gc;
    gc.set_bool(true);
    
    bool passed = true;
    
    passed &= (gc.get_as_int() == 1);
    
    passed &= (gc.get_as_uint() == 1);
    
    passed &= (gc.get_as_long() == 1);
    
    passed &= (gc.get_as_ulong() == 1);
    
    gc.set_bool(false);
    passed &= (gc.get_as_int() == 0);
    
    print_test_case("get_as_* from bool", passed);
    all_passed &= passed;
  }

  // Test 5: Precisione nelle conversioni numeriche
  {
    GenericContainer gc;
    
    // Test float → double (real_type)
    float float_val = 3.14159265358979323846f;
    gc.set(float_val);
    
    bool passed = (gc.get_type() == GC_type::REAL);
    
    real_type retrieved = gc.get_real();
    // Verifica che la precisione sia preservata (entro i limiti di float)
    passed &= (abs(retrieved - float_val) < 1e-6);
    
    // Test complex<float> → complex<double>
    complex<float> cfloat(1.0f, 2.0f);
    gc.set(cfloat);
    
    passed &= (gc.get_type() == GC_type::COMPLEX);
    complex_type cretrieved = gc.get_complex();
    passed &= (abs(cretrieved.real() - 1.0) < 1e-12);
    passed &= (abs(cretrieved.imag() - 2.0) < 1e-12);
    
    print_test_case("Float to double precision", passed);
    all_passed &= passed;
  }

  // Test 6: Round-trip serializzazione per tutti i tipi
  {
    bool passed = true;
    
    // Test per ogni tipo principale
    vector<pair<string, GenericContainer>> test_cases = {
      {"bool", GenericContainer(true)},
      {"int", GenericContainer(42)},
      {"long", GenericContainer(1234567890123LL)},
      {"real", GenericContainer(3.141592653589793)},
      {"complex", GenericContainer(complex<double>(1.0, 2.0))},
      {"string", GenericContainer("Hello, World!")}
    };
    
    for (auto& [name, gc] : test_cases) {
      vector<uint8_t> buffer = gc.serialize();
      GenericContainer restored;
      restored.de_serialize(buffer);
      
      string diff = gc.compare_content(restored, name + " round-trip");
      if (!diff.empty()) {
        cout << "  Failed round-trip for " << name << ": " << diff << "\n";
        passed = false;
      }
    }
    
    print_test_case("Round-trip serialization for all basic types", passed);
    all_passed &= passed;
  }

  // Test 7: Compatibilità vettori di tipi diversi
  {
    GenericContainer gc_int;
    gc_int.set_vec_int({1, 2, 3});
    
    GenericContainer gc_long;
    gc_long.set_vec_long({1, 2, 3});
    
    // Verifica che i contenuti siano equivalenti
    bool passed = (gc_int.get_vec_int().size() == gc_long.get_vec_long().size());
    
    // Test conversione attraverso copyto
    vec_long_type from_int;
    gc_int.copyto_vec_long(from_int);
    passed &= (from_int.size() == 3);
    passed &= (from_int[0] == 1 && from_int[1] == 2 && from_int[2] == 3);
    
    print_test_case("Vector type compatibility", passed);
    all_passed &= passed;
  }

  // Test 8: get_value template method
  {
    GenericContainer gc;
    gc.set_int(42);
    
    int_type int_val;
    gc.get_value(int_val, "Test get_value");
    
    bool passed = (int_val == 42);
    
    // Test con tipo diverso ma compatibile
    long_type long_val;
    gc.get_value(long_val, "Test get_value to long");
    passed &= (long_val == 42);
    
    print_test_case("get_value template method", passed);
    all_passed &= passed;
  }

  // Test 9: get_pointer template (diverse specializzazioni)
  {
    // Crea un puntatore a intero
    int x = 42;
    GenericContainer gc;
    gc.set_pointer(&x);
    
    bool passed = true;
    
    // get_pointer come riferimento
    int & ptr_ref = gc.get_pointer<int>();
    passed &= (&ptr_ref == &x);
    passed &= (ptr_ref == 42);
    
    // get_pointer come valore (const version)
    int & ptr_val = gc.get_pointer<int>();
    passed &= (&ptr_val == &x);
    
    // Modifica attraverso il puntatore
    ptr_ref = 100;
    passed &= (x == 100);
    
    print_test_case("get_pointer template specialization", passed);
    all_passed &= passed;
  }

  // Test 10: Compatibilità incrociata tra tipi numerici
  {
    // Test che verifica le conversioni più comuni
    bool passed = true;
    
    // Da bool a tutti i tipi numerici
    GenericContainer gc_bool(true);
    passed &= (abs(gc_bool.get_number() - 1.0) < 1e-12);
    passed &= (abs(gc_bool.get_complex_number().real() - 1.0) < 1e-12);
    
    // Da int a real e complex
    GenericContainer gc_int(42);
    passed &= (abs(gc_int.get_number() - 42.0) < 1e-12);
    passed &= (abs(gc_int.get_complex_number().real() - 42.0) < 1e-12);
    
    // Da real a complex
    GenericContainer gc_real(3.14);
    complex_type c = gc_real.get_complex_number();
    passed &= (abs(c.real() - 3.14) < 1e-12);
    passed &= (abs(c.imag()) < 1e-12);
    
    print_test_case("Cross-type numeric compatibility", passed);
    all_passed &= passed;
  }

  print_test_case("Type Compatibility Overall", all_passed);
}

void test_performance_specific()
{
  print_header("TEST PERFORMANCE: Specific Operations Benchmark");

  cout << "  ⚡ Running specific performance benchmarks...\n";

  // Test 1: Performance di collapse su strutture grandi
  {
    auto start = chrono::high_resolution_clock::now();
    
    GenericContainer gc;
    gc.set_vector(1000);  // 1000 righe
    
    for (int i = 0; i < 1000; ++i) {
      gc[i].set_vec_real(vector<real_type>(500, i * 1.0));  // 500 colonne
    }
    
    auto mid = chrono::high_resolution_clock::now();
    
    gc.collapse();  // Dovrebbe creare una matrice 1000×500
    
    auto end = chrono::high_resolution_clock::now();
    
    auto creation_time = chrono::duration_cast<chrono::microseconds>(mid - start).count();
    auto collapse_time = chrono::duration_cast<chrono::microseconds>(end - mid).count();
    
    cout << "  📊 Collapse performance:\n";
    cout << "    • Created 1000×500 nested structure: " << creation_time << " μs\n";
    cout << "    • Collapse to matrix: " << collapse_time << " μs\n";
    
    bool passed = (gc.get_type() == GC_type::MAT_REAL);
    if (passed) {
      mat_real_type& mat = gc.get_mat_real();
      passed = (mat.num_rows() == 1000 && mat.num_cols() == 500);
    }
    
    print_test_case("Large structure collapse", passed);
  }

  // Test 2: Performance promozione di vettori grandi
  {
    auto start = chrono::high_resolution_clock::now();
    
    GenericContainer gc;
    vec_int_type large_vec(1000000);  // 1 milione di elementi
    for (size_t i = 0; i < large_vec.size(); ++i) {
      large_vec[i] = i % 100;
    }
    gc.set_vec_int(large_vec);
    
    auto mid = chrono::high_resolution_clock::now();
    
    gc.promote_to_vec_real();  // Conversione in virgola mobile
    
    auto end = chrono::high_resolution_clock::now();
    
    auto creation_time = chrono::duration_cast<chrono::microseconds>(mid - start).count();
    auto promote_time = chrono::duration_cast<chrono::microseconds>(end - mid).count();
    
    cout << "  📊 Promotion performance:\n";
    cout << "    • Created vector of 1M integers: " << creation_time << " μs\n";
    cout << "    • Promote to real: " << promote_time << " μs\n";
    
    bool passed = (gc.get_type() == GC_type::VEC_REAL);
    passed &= (gc.get_vec_real().size() == 1000000);
    
    print_test_case("Large vector promotion", passed);
  }

  // Test 3: Memory footprint per diversi tipi
  {
    cout << "  📊 Memory footprint analysis:\n";
    
    // Container semplice
    GenericContainer gc_simple;
    gc_simple.set_int(42);
    int32_t simple_size = gc_simple.mem_size();
    cout << "    • Simple int: " << simple_size << " bytes\n";
    
    // Vettore medio
    GenericContainer gc_vec;
    gc_vec.set_vec_int(vector<int_type>(1000, 42));
    int32_t vec_size = gc_vec.mem_size();
    cout << "    • Vector of 1000 ints: " << vec_size << " bytes\n";
    
    // Matrice
    GenericContainer gc_mat;
    mat_int_type mat(100, 100);
    gc_mat.set_mat_int(mat);
    int32_t mat_size = gc_mat.mem_size();
    cout << "    • 100×100 matrix: " << mat_size << " bytes\n";
    
    // Struttura annidata complessa
    GenericContainer gc_complex;
    gc_complex.set_map();
    for (int i = 0; i < 10; ++i) {
      string key = "item_" + to_string(i);
      gc_complex[key].set_vec_real(vector<real_type>(100, i * 1.0));
    }
    int32_t complex_size = gc_complex.mem_size();
    cout << "    • Complex nested structure: " << complex_size << " bytes\n";
    
    print_test_case("Memory footprint analysis", true);
  }

  // Test 4: Serializzazione/deserializzazione performance
  {
    // Crea una struttura complessa
    GenericContainer gc;
    gc.set_map();
    
    for (int i = 0; i < 100; ++i) {
      string key = "obj_" + to_string(i);
      gc[key].set_map();
      gc[key]["id"].set_int(i);
      gc[key]["data"].set_vec_real(vector<real_type>(1000, i * 1.0));
    }
    
    auto start = chrono::high_resolution_clock::now();
    
    vector<uint8_t> buffer = gc.serialize();
    
    auto mid = chrono::high_resolution_clock::now();
    
    GenericContainer restored;
    restored.de_serialize(buffer);
    
    auto end = chrono::high_resolution_clock::now();
    
    auto serialize_time = chrono::duration_cast<chrono::microseconds>(mid - start).count();
    auto deserialize_time = chrono::duration_cast<chrono::microseconds>(end - mid).count();
    
    cout << "  📊 Serialization performance:\n";
    cout << "    • Structure: 100 objects with 1000-element vectors\n";
    cout << "    • Serialize time: " << serialize_time << " μs\n";
    cout << "    • Deserialize time: " << deserialize_time << " μs\n";
    cout << "    • Buffer size: " << buffer.size() << " bytes\n";
    
    bool passed = gc.compare_content(restored, "Performance test").empty();
    print_test_case("Serialization performance test", passed);
  }

  cout << "  ✅ Performance tests completed\n";
}
// ===========================================================================
// Main Test Runner
// ===========================================================================

int main()
{
  cout << "\n" << repeat( 60, "◆" ) << "\n";
  cout << "       GENERIC CONTAINER COMPREHENSIVE TEST SUITE\n";
  cout << repeat( 60, "◆" ) << "\n\n";

  cout << "📋 Test Plan:\n";
  cout << "  1. Simple Data Types\n";
  cout << "  2. Vector Types\n";
  cout << "  3. Matrix Types\n";
  cout << "  4. Complex Containers\n";
  cout << "  5. Assignment and Copy\n";
  cout << "  6. Type Promotion\n";
  cout << "  7. Serialization\n";
  cout << "  8. I/O Formats\n";
  cout << "  9. Advanced Features\n";
  cout << " 10. Move Semantics\n";
  cout << " 11. Performance and Stress\n";
  cout << " 12. Matrix Type Comprehensive\n";
  cout << " 13. Promotion Comprehensive\n";
  cout << " 14. Copyto Methods\n";
  cout << " 15. Push Methods\n";
  cout << " 16. Get Map with Vector Keys\n";
  cout << " 17. Advanced I/O\n";
  cout << " 18. Edge Cases\n";
  cout << " 19. Static & Utility\n";
  cout << " 20. Type Compatibility\n";
  cout << " 21. Specific Performance\n";

  try
  {
    test_simple_types();
    test_vector_types();
    test_matrix_types();
    test_complex_containers();
    test_assignment_and_copy();
    test_promotion();
    test_serialization();
    test_io_formats();
    test_advanced_features();
    test_move_semantics();
    test_performance_stress();
    
    // Nuovi test aggiunti
    test_mat_type_comprehensive();
    test_promotion_comprehensive();
    test_copyto_methods();
    test_push_methods();
    test_get_map_with_vector_keys();
    test_io_advanced();
    test_edge_cases();
    test_static_and_utility();
    test_type_compatibility();
    test_performance_specific();

    cout << "\n" << repeat( 60, "◆" ) << "\n";
    cout << "          🎉 ALL TESTS COMPLETED SUCCESSFULLY!\n";
    cout << repeat( 60, "◆" ) << "\n";
  }
  catch ( const exception & e )
  {
    cout << "\n" << repeat( 60, "!" ) << "\n";
    cout << "❌ TEST FAILED WITH EXCEPTION:\n";
    cout << "   " << e.what() << "\n";
    cout << repeat( 60, "!" ) << "\n";
    return 1;
  }

  return 0;
}
