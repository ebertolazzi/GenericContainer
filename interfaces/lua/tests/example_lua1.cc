/*--------------------------------------------------------------------------*\
 |                                                                          |
 |  Copyright (C) 2013                                                      |
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
 \example example8.cc

 Example of use of Lua interface. `GenericContainer` is used
 to read a lua table initialied in a lua script.
 */

#include "GenericContainer/GenericContainerInterface_lua.hh"
#include <cmath>
#include <iostream>
#include <fstream>

using namespace std;
using namespace GC;

int main()
{
  int status = 0;
  cout << "\n\n\n"
       << "***********************\n"
       << "   lua example N.1     \n"
       << "***********************\n\n";

  try
  {
    GenericContainer gc;
    LuaInterpreter   lua;
    char const *     script = "test.lua";
    ifstream         probe( script );
    if ( !probe.good() ) script = "./src_lua_interface/tests/test.lua";
    lua.do_file( script );
    lua.global_to_GC( "DATA", gc );

    bool passed = gc["mesh_files"].get_vec_string().size() == 4;
    passed &= gc["mesh_files"].get_vec_string()[1] == "granny_toro.2.neutral.bz2";
    passed &= gc["do_save"].get_vec_bool()[3] == false;
    passed &= std::abs( gc["mu"].get_real() - 1.25e-6 ) < 1e-18;
    passed &= gc["regions"][1][0].get_string() == "granny";
    if ( !passed ) throw runtime_error( "lua example N.1 validation failed" );

    cout << "\n\n\n\nConverted in GenericContainer\n\n";
    gc.dump( cout );
  }
  catch ( std::exception & exc )
  {
    cout << exc.what() << '\n';
    status = 1;
  }
  catch ( ... )
  {
    cout << "Unknonwn error\n";
    status = 1;
  }

  cout << "ALL DONE!\n\n\n\n";
  return status;
}
