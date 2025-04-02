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

 \example example16.cc

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
    ifstream file( "examples/test.json" );
    gc1.from_json( file );
    file.close();
  }
  std::cout << "WRITE YAML\n";
  {
    ofstream file( "examples/test.yml" );
    gc1.to_yaml( file );
    file.close();
  }
  std::cout << "READ YAML\n";
  {
    ifstream file( "examples/test.yml" );
    gc2.from_yaml( file );
    file.close();
  }
  std::cout << "WRITE YAML\n";
  {
    ofstream file( "examples/test2.yml" );
    gc2.to_yaml( file );
    file.close();
  }
  std::cout << "READ YAML\n";
  {
    ifstream file( "examples/test3.yml" );
    gc2.from_yaml( file );
    file.close();
  }

  //std::cout << "GC1\n";
  //gc1.print(cout);
  //std::cout << "GC2\n";
  //gc2.print(cout);

  cout << "\n\nAll done Folks!!!\n\n";
  return 0;
}

#endif
