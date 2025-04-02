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

 \example example13.cc

 */

#include "GenericContainer.hh"
#include "GenericContainer/GenericContainerInterface_json.hh"

#include <iostream>
#include <fstream>

using namespace std;
using namespace GC;

int
main() {

  cout
    << "\n\n\n"
    << "***********************\n"
    << "      example N.13     \n"
    << "***********************\n\n";

  try {
    // read JSON file and convert to generic container
    GenericContainer gc;
    string fname{ "examples/data.json" };
    bool ok = file_JSON_to_GC( fname, gc );
    if ( !ok ) std::cerr << "Failed to parse: " << fname << '\n';
    std::cout << "\n\n\n\nGC\n\n";
    gc.print(std::cout);
    std::cout << "\n\n\n\nYAML\n\n";
    gc.to_yaml( std::cout );
    std::cout << "\n\n\n\nJSON\n\n";
    gc.to_json( std::cout );
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
