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
 to call a lua function and read the results.

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
       << "   lua example N.3     \n"
       << "***********************\n\n";

  try
  {
    LuaInterpreter lua;
    char const * script = "test_call.lua";
    ifstream     probe( script );
    if ( !probe.good() ) script = "./src_lua_interface/tests/test_call.lua";
    lua.do_file( script );

    GC::GenericContainer gc, gc_res;
    gc["function"]              = "pippo";
    GC::GenericContainer & vec  = gc["args"];
    vec[0]                      = 12;
    vec[1]                      = 13;
    vec[2]                      = "aaa";
    GC::GenericContainer & map  = vec[3];
    map["nonna"]                = "papera";
    GC::GenericContainer & vec1 = map["abc123"];
    vec1[0]                     = 12.3;
    vec1[1]                     = "a string";
    vec1[2]                     = 1;
    // dump(cout);
    lua.call( gc, gc_res );

    bool passed = std::abs( gc_res["sol"].get_number() - 156.0 ) < 1e-12;
    passed &= gc_res["cpy"]["nonna"].get_string() == "papera";
    passed &= gc_res["cpy"]["abc123"][1].get_string() == "a string";
    passed &= gc_res["aaa"]["vec"].get_num_elements() == 4;
    passed &= std::abs( gc_res["aaa"]["vec"].get_number_at( 3 ) - 4.0 ) < 1e-12;
    passed &= std::abs( gc_res["aaa"]["map"]["c"].get_number() - 3.0 ) < 1e-12;
    if ( !passed ) throw runtime_error( "lua example N.3 validation failed" );

    cout << "Result:\n";
    gc_res.dump( cout );
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
