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

 \example example11.cc

 */

#include "GenericContainer.hh"
#include "GenericContainer/GenericContainerYamlInterface.hh"

#include <iostream>
#include <fstream>

using namespace std;
using namespace GC;

int
main() {

  cout
    << "\n\n\n"
    << "***********************\n"
    << "      example N.11     \n"
    << "***********************\n\n";

  try {
    // read YAML file and convert to generic container
    GenericContainer gc;
    string fname{ "examples/data.yml" };
    bool ok = file_YAML_to_GC( fname, gc );
    if ( !ok ) std::cerr << "Failed to parse: " << fname << '\n';
    std::cout << "\n\n\n\nGC\n\n";
    gc.print(std::cout);
    std::cout << "\n\n\n\nYAML\n\n";
    gc.to_yaml( std::cout );
    std::cout << "\n\n\n\n";
  }
  catch ( std::exception & exc ) {
    cout << exc.what() << '\n';
  }
  catch (...) {
    cout << "Unknonwn error\n";
  }

  cout << "ALL DONE!\n\n\n\n";
}
