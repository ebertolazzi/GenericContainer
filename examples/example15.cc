/*--------------------------------------------------------------------------*\
 |                                                                          |
 |  Copyright (C) 2021                                                      |
 |                                                                          |
 |         , __                 , __                                        |
 |        /|/  \               /|/  \                                       |
 |         | __/ _   ,_         | __/ _   ,_                                |
 |         |   \|/  /  |  |   | |   \|/  /  |  |   |                        |
 |         |(__/|__/   |_/ \_/|/|(__/|__/   |_/ \_/|/                       |
 |                           /|                   /|                        |
 |                           \|                   \|                        |
 |                                                                          |
 |      Enrico Bertolazzi                                                   |
 |      Dipartimento di Ingegneria Industriale                              |
 |      Università degli Studi di Trento                                    |
 |      email: enrico.bertolazzi@unitn.it                                   |
 |                                                                          |
\*--------------------------------------------------------------------------*/
/*!

 \example example15.cc

 */
#include "GenericContainer/GenericContainerInterface_json.hh"
#include <iostream>
#include <string>
#include <chrono>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

using namespace GC;
using namespace std;

int
//main( int argc, const char * argv[] ) {
main() {

  // Definizione dei tipi di tempo
  using std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  using std::chrono::milliseconds;


  cout
    << "\n\n\n"
    << "***********************\n"
    << "      example N.15     \n"
    << "***********************\n\n";

  GenericContainer gc1;
  GenericContainer gc2;
  GenericContainer gc3;

  // Memorizza il tempo iniziale
  auto start = high_resolution_clock::now();
  {
    //ifstream file( "examples/large-file.json" );
    ifstream file( "examples/data2.json" );
    gc1.from_json( file );
    file.close();
  }

  auto end      = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(end - start);
  std::cout << "READ JSON to GC1 " << duration.count() << " millisecondi." << std::endl;

  start = high_resolution_clock::now();
  {
    //ifstream file( "examples/large-file.json" );
    ifstream file( "examples/data2.json" );
    gc2.from_json2( file );
    file.close();
  }

  end      = high_resolution_clock::now();
  duration = duration_cast<milliseconds>(end - start);
  std::cout << "READ JSON to GC2 " << duration.count() << " millisecondi." << std::endl;

  cout << "COMPARE GC1 vs GC2: " << gc1.compare_content(gc2,">>> ");

  start = high_resolution_clock::now();
  vector<uint8_t> buffer( static_cast<size_t>( gc1.mem_size() ) );
  int32_t nb = gc1.serialize( buffer );

  end      = high_resolution_clock::now();
  duration = duration_cast<milliseconds>(end - start);
  std::cout << "SERIALIZE [" << nb << "] bytes " << duration.count() << " millisecondi." << std::endl;



  start = high_resolution_clock::now();
  gc3.clear();
  int32_t nb2 = gc3.de_serialize( buffer );

  end      = high_resolution_clock::now();
  duration = duration_cast<milliseconds>(end - start);
  std::cout << "DE-SERIALIZE to GC3 [" << nb2 << "] bytes " << duration.count() << " millisecondi." << std::endl;


  cout << "COMPARE GC1 vs GC3: " << gc1.compare_content(gc2);


  cout << "\n\nAll done Folks!!!\n\n";
  return 0;
}

#endif
