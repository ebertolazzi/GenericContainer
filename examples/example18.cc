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

 \example example18.cc

 */
#include "GenericContainer/GenericContainerInterface_toml.hh"
#include <iostream>
#include <string>
#include <chrono>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

using namespace GC;
using namespace std;

#include <iostream>
#include <sstream>

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
    << "      example N.18     \n"
    << "***********************\n\n";

  GenericContainer gc;

  std::cout << "READ TOML\n";
  {
    ifstream file( "examples/settings.toml" );
    gc.from_toml( file );
    file.close();
  }
  std::cout << "\n\n\nWRITE TOML\n";
  gc.to_toml( std::cout );

  cout << "\n\nAll done Folks!!!\n\n";
  return 0;
}

#endif
