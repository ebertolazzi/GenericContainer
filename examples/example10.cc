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

 \example example10.cc

 */

#include "GenericContainer.hh"
#include <iostream>
#include <fstream>

using namespace std;
using namespace GC;

int
main() {

  cout
    << "\n\n\n"
    << "***********************\n"
    << "      example N.10     \n"
    << "***********************\n\n";

  try {
    GenericContainer gc1, gc2, gc3, gc4, pars;
    gc1.readFormattedData( "examples/example10_data.txt" );
    gc2.readFormattedData2( "examples/example10_data.txt", "#", " \t", &pars );
    gc2.to_gc(gc3);
    gc4.from_gc(gc2);
    std::cout << "GC1 --------------------------\n";
    gc1.print(std::cout);
    std::cout << "GC2 --------------------------\n";
    gc2.print(std::cout);
    std::cout << "PARS --------------------------\n";
    pars.print(std::cout);
    std::cout << "GC2 COPY ----------------------\n";
    gc3.print(std::cout);
    std::cout << "GC2 COPY2 ---------------------\n";
    gc4.print(std::cout);
  }
  catch ( std::exception & exc ) {
    cout << exc.what() << '\n';
  }
  catch (...) {
    cout << "Unknonwn error\n";
  }

  cout << "ALL DONE!\n\n\n\n";
}
