/*--------------------------------------------------------------------------*\
 |                                                                          |
 |  Copyright (C) 2026                                                      |
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

#include "GenericContainer/GenericContainerInterface_nlohmann.hh"

#include <iomanip>

namespace GC_namespace
{

  void GenericContainer::to_json( ostream_type & stream, string_view prefix ) const
  {
    nlohmann::json const json = *this;
    string_type          text = json.dump( 2 );
    if ( prefix.empty() )
    {
      stream << text;
      return;
    }

    std::size_t pos{ 0 };
    while ( pos < text.size() )
    {
      auto const eol{ text.find( '\n', pos ) };
      stream << prefix << text.substr( pos, eol == string_type::npos ? string_type::npos : eol - pos );
      if ( eol == string_type::npos ) break;
      stream << '\n';
      pos = eol + 1;
    }
  }

  bool GenericContainer::from_json( istream_type & stream )
  {
    try
    {
      nlohmann::json const json = nlohmann::json::parse( stream );
      *this                     = json.get<GenericContainer>();
      return true;
    }
    catch ( std::exception const & e )
    {
      std::cerr << "GenericContainer::from_json: " << e.what() << '\n';
      return false;
    }
    catch ( ... )
    {
      std::cerr << "GenericContainer::from_json: failed\n";
      return false;
    }
  }

  bool GenericContainer::from_json2( istream_type & stream ) { return this->from_json( stream ); }

}  // namespace GC_namespace
