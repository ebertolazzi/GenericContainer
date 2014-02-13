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

//
// file: GenericContainerTables.cc
//

#include "GenericContainer.hh"
#include <iomanip>
#include <string>

namespace GC {

  using namespace std ;

  void
  writeTable( vec_string_type const & headers,
              vector_type     const & data,
              std::ostream          & stream,
              char const              delimiter ) {

    unsigned ncol = (unsigned)headers.size() ;
    unsigned nrow = (unsigned)data[0].get_num_elements() ;

    stream << headers[0] ;
    for ( unsigned icol = 1 ; icol < ncol ; ++icol )
      stream << delimiter << headers[icol] ;
    stream << '\n' ;

    for ( unsigned row = 0 ; row < nrow ; ++row ) {
      stream << data[0].get_number(row) ;
      for ( unsigned icol = 1 ; icol < ncol ; ++icol )
        stream << delimiter << data[icol].get_number(row)  ;
      stream << '\n' ;
    }
  }

  void
  writeTableFormatted( vec_string_type const & headers,
                       vector_type     const & data,
                       std::ostream          & stream ) {

    int ncol = (int)headers.size() ;
    int nrow = (int)data[0].get_num_elements() ;

    if ( ncol == 0 ) return ;

    // calcolo lunghezza massima stringhe headers
    int ml = 0 ;
    vec_string_type::const_iterator is = headers.begin() ;
    for ( ; is != headers.end() ; ++is )
      if ( ml < (int)is->length() ) ml = (int)is->length() ;
    // taglio a lunghezza min/max
    if      ( ml < 8  ) ml = 8 ;
    else if ( ml > 20 ) ml = 20 ;

    string line = std::string(ncol*(ml+1)-1, '-') ;

    is = headers.begin() ;
    stream << std::setw(ml) << *is ;
    for ( ++is ; is != headers.end() ; ++is )
      stream << " " << std::setw(ml) << *is ;
    stream << '\n' << line << '\n';

    for ( int row = 0 ; row < nrow ; ++row ) {
      stream << std::setw(ml) << data[0].get_number(row) ;
      for ( int icol = 1 ; icol < ncol ; ++icol )
        stream << " " << std::setw(ml) << data[icol].get_number(row)  ;
      stream << '\n' ;
    }
    stream << line << '\n';
  }


}

//
// eof: GenericContainerTables.cc
//
