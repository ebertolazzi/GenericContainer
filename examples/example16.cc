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
    << "      example N.16     \n"
    << "***********************\n\n";

  GenericContainer gc1;
  GenericContainer gc2;

  std::cout << "READ JSON\n";
  {
    ifstream file( "examples/data.json" );
    gc1.from_json( file );
    file.close();
  }
  std::cout << "WRITE YAML\n";
  {
    ofstream file( "examples/data.yaml" );
    gc1.to_yaml( file );
    file.close();
  }
  std::cout << "REAL YAML\n";
  {
    ifstream file( "examples/data.yaml" );
    gc2.from_yaml( file );
    file.close();
  }
  std::cout << "WRITE YAML\n";
  {
    ofstream file( "examples/data2.yaml" );
    gc2.to_yaml( file );
    file.close();
  }

  cout << "\n\nAll done Folks!!!\n\n";
  return 0;
}

#endif
