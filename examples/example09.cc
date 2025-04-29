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

 \example example9.cc

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
    << "      example N.9      \n"
    << "***********************\n\n";

  try {
    GC::GenericContainer gc, gc_new;
    GC::GenericContainer & gc1 = gc["pippo"];
    GC::GenericContainer & gc2 = gc["pluto"];
    GC::GenericContainer & gc3 = gc["paperino"];
    GC::GenericContainer & gc4 = gc["vector"];
    gc1.set_mat_real(2,2);
    gc1.get_real_at(0,0) = 22323;
    gc1.get_real_at(0,1) = 4443;
    gc1.get_real_at(1,0) = 432;
    gc1.get_real_at(1,1) = 433;
    gc2.set_vec_string();
    gc2.push_string("nonna");
    gc2.push_string("papera");
    gc3 = "pippo";
    gc4[0] = 1.234;
    gc4[1] = true;
    vec_real_type & v = gc4[2].set_vec_real(4);
    v[0] = 1;
    v[1] = -1;
    v[2] = 2;
    v[3] = 4;
    gc4[3] = "superkaly";
    gc4[4] = complex_type(1,2);
    int sz = gc.mem_size();
    cout << "Size: " << sz << '\n';

    vector<uint8_t> buffer( static_cast<size_t>( sz ) );
    int sz1 = gc.serialize( sz, &buffer.front() );
    cout << "Size1: " << sz1 << '\n';

    //for ( auto c : buffer ) cout << (int)c << '\n';
    std::cout << "--------------------------\n";
    gc.print(std::cout);
    gc_new.de_serialize( sz, &buffer.front() );
    std::cout << "--------------------------\n";
    gc_new.print(std::cout);
  }
  catch ( std::exception & exc ) {
    cout << exc.what() << '\n';
  }
  catch (...) {
    cout << "Unknonwn error\n";
  }

  cout << "ALL DONE!\n\n\n\n";
}
