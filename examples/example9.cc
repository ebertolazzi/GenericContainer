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
 |      Universita` degli Studi di Trento                                   |
 |      email: enrico.bertolazzi@unitn.it                                   |
 |                                                                          |
\*--------------------------------------------------------------------------*/
/*!
 \example example8.cc

 Example of use of Lua interface. `GenericContainer` is used
 to write a lua table initialied in a C++ code.
 */

#include "GenericContainerLuaInterface.hh"
#include <iostream>
#include <fstream>

using namespace std ;

void
gc_set( GenericContainer & gc ) {
  GenericContainer::vector_type & v = gc.set_vector() ;
  v.resize(10) ;
  v[0] = 1 ;
  v[1].set_vec_real() ;
  v[2].set_map() ;
  v[3].set_vec_string() ;
  v[4] = 1.3 ;
  v[5] = "pippo" ;
  v[6].set_map() ;
  v[7].set_vector() ;
  v[8] = true ;
  GenericContainer::vec_real_type & vv = v[1].get_vec_real() ;
  vv.resize(10) ;
  vv[2] = 123 ;
  GenericContainer::map_type & mm = v[2].get_map() ;
  mm["pippo"]    = 13 ;
  mm["pluto"]    = 1  ;
  mm["paperino"] = 3  ;
  GenericContainer & gmm = v[2] ; // access element 2 as GenericContainer
  gmm["aaa"]     = "stringa1"  ; // is the same as mm["aaa"] = "stringa"
  gmm["bbb"]     = "stringa2"  ; // is the same as mm["aaa"] = "stringa"
  GenericContainer::vec_string_type & vs = v[3].get_vec_string() ;
  vs.push_back("string1");
  vs.push_back("string2");
  vs.push_back("string3");
  vs.push_back("string4");
  GenericContainer::map_type & m = v[6].get_map() ;
  m["aaa"]    = 123 ;
  m["bbb"]    = 3.4 ;
  m["vector"].set_vec_int() ;
  GenericContainer::vec_int_type & vi = m["vector"].get_vec_int() ;
  vi.push_back(12) ;
  vi.push_back(10) ;
  vi.push_back(1) ;

  GenericContainer::vector_type & vg = v[7].get_vector() ;
  vg.resize(3) ;
  vg[0] = 123 ;
  vg[1] = 3.14 ;
  vg[2] = "nonna papera" ;
}

int
main() {

  cout << "\n\n\n"
       << "***********************\n"
       << "      example N.8      \n"
       << "***********************\n\n" ;

  try {
    GenericContainer gc ;
    LuaInterpreter   lua ;

    gc.clear() ;
    gc_set( gc ) ;
    gc.print(cout) ;
    cout << "\n\n\n\nConverted in lua\n\n" ;
    lua.GC_to_global( gc, "DATA" ) ;
    lua.do_file("test_print.lua") ;
  }
  catch ( std::exception & exc ) {
    cout << exc.what() << '\n'  ;
  }
  catch (...) {
    cout << "Unknonwn error\n" ;
  }
  
  cout << "ALL DONE!\n\n\n\n" ;
}