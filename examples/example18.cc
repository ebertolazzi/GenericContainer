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

int
// main( int argc, const char * argv[] ) {
main()
{
  // Definizione dei tipi di tempo
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;

  std::cout << "\n\n\n"
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

  GC::GenericContainer gc_copy_from;
  gc_copy_from["Pippo"]    = 9.0;
  gc_copy_from["Paperino"] = 32.9;
  gc_copy_from["Pluto"]    = 7.7;

  std::cout << "Assume we have this GC to copy from:\n\n";
  gc_copy_from.dump( std::cout );

  GC::GenericContainer gc_copy_to;
  gc_copy_to["Pippo"]                  = 3.0;
  gc_copy_to["Paperino"]               = 2.9;
  gc_copy_to["Pluto"]                  = 0.7;
  gc_copy_to["KeyNotPresentInOtherGC"] = "ciao";

  std::cout << "\nmerge:\n";
  gc_copy_from.dump( std::cout );

  std::cout << "\ninto:\n";
  gc_copy_to.dump( std::cout );

  std::cout << "\nresult:\n";
  gc_copy_to.merge( gc_copy_from, "merge gc_copy_from->gc_copy_to" );
  gc_copy_to.dump( std::cout );

  std::cout << "\ncopy:\n";
  gc_copy_from.dump( std::cout );

  std::cout << "\ninto:\n";
  gc_copy_to.dump( std::cout );

  std::cout << "\nresult:\n";
  gc_copy_to = gc_copy_from;
  gc_copy_to.dump( std::cout );

  cout << "\n\nAll done Folks!\n\n";
  return 0;
}

#endif
