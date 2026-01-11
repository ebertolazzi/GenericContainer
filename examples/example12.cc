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

 \example example12.cc

 */

#include "GenericContainer.hh"
#include "GenericContainer/GenericContainerInterface_yaml.hh"

#include <iostream>
#include <fstream>

using namespace std;
using namespace GC;

int main()
{
  cout << "\n\n\n"
       << "***********************\n"
       << "      example N.12     \n"
       << "***********************\n\n";

  try
  {
    // real yaml file to generic container
    GenericContainer gc;

    auto & m1 = gc["matrix_int"].set_mat_int( 3, 4 );
    auto & m2 = gc["matrix_real"].set_mat_real( 3, 2 );
    auto & m3 = gc["matrix_complex"].set_mat_complex( 4, 2 );
    auto & v3 = gc["vector_complex"].set_vec_complex( 3 );

    m1( 2, 1 ) = 4;
    m2( 2, 1 ) = 2;
    m3( 2, 1 ) = 1;
    m3( 0, 0 ) = GenericContainer::complex_type( 4, 5 );

    v3[0] = 1;
    v3[1] = GenericContainer::complex_type( 55, 1 );
    v3[2] = GenericContainer::complex_type( 0, -2 );

    std::cout << "\n\n\n\nYAML\n\n" << gc.to_yaml() << "\n\n\n\n";
  }
  catch ( std::exception & exc )
  {
    cout << exc.what() << '\n';
  }
  catch ( ... )
  {
    cout << "Unknonwn error\n";
  }

  cout << "ALL DONE!\n\n\n\n";
}
