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

 \example example16.cc

 */
#include "GenericContainer/GenericContainer.hh"
#include <iostream>
#include <string>
#include <chrono>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

using namespace GC;
using namespace std;

int
//main( int argc, const char * argv[] ) {
main() {


  cout
    << "\n\n\n"
    << "***********************\n"
    << "      example N.17     \n"
    << "***********************\n\n";

  GenericContainer gc1;
  GenericContainer gc2;
  GenericContainer gc3;
  GenericContainer gc4;
  
  gc3["pippo"] = true;
  gc2["pluto"] = gc3;

  gc1["paperino"] = gc2;
  gc4["blue"] = 23;
  gc1["aabb"] = gc4;
  
  std::cout << "gc --> " << gc1.get_map_bool( {"paperino","pluto","pippo"} ) << "\n";
  std::cout << "gc --> " << gc1.get_map_number( {"aabb","blue"} ) << "\n";

  cout << "\n\nAll done Folks!!!\n\n";
  return 0;
}

#endif
