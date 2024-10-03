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
 |      Universita` degli Studi di Trento                                   |
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

  // Memorizza il tempo iniziale
  auto start = high_resolution_clock::now();
  {
    ifstream file( "examples/large-file.json" );
    //ifstream file( "examples/data.json" );
    gc1.from_json( file );
    file.close();
  }

  auto end      = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(end - start);
  std::cout << "GC1 " << duration.count() << " millisecondi." << std::endl;

  start = high_resolution_clock::now();
  {
    ifstream file( "examples/large-file.json" );
    //ifstream file( "examples/data.json" );
    gc2.from_json2( file );
    file.close();
  }

  end      = high_resolution_clock::now();
  duration = duration_cast<milliseconds>(end - start);
  std::cout << "GC2 " << duration.count() << " millisecondi." << std::endl;


  cout << "\n\nGC1\n\n";
  //gc1.print( cout );
  gc1(2)("payload")("commits")(0).print(cout);

  cout << "\n\nGC2\n\n";
  //gc2.print( cout );
  gc2(2)("payload")("commits")(0).print(cout);

  cout << "COMPARE: " << gc1.compare_content(gc2);

  cout << "\n\nAll done Folks!!!\n\n";
  return 0;
}

#endif
