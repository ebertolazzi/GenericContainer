//
//  main.cpp
//  genconjson
//
//  Created by Nicola Dal Bianco on 08/11/17.
//  Copyright © 2017 Nicola Dal Bianco. All rights reserved.
//

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif

#include "GenericContainer/GenericContainer.hh"

#include <fstream>
#include <string>
#include <iomanip>
#include <regex>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"  // Per formattare l'output
#include "rapidjson/filewritestream.h"
#endif

namespace GC_namespace
{

  using std::ifstream;
  using std::ofstream;
  using std::regex;
  using std::smatch;
  using std::string;

  // Funzione ricorsiva per convertire un JSON in un GenericContainer
  static bool decode_json( GenericContainer & gc, rapidjson::Value const & value )
  {
    // Regex per riconoscere numeri complessi (es: "3.5+4.2i" o "-2.5-3.1i")
    static regex complex_regex( R"(^([+-]?\d+(\.\d+)?)[+-](\d+(\.\d+)?)i$)" );

    bool ok{ true };
    if ( value.IsNull() ) { gc.clear(); }
    else if ( value.IsBool() ) { gc = value.GetBool(); }
    else if ( value.IsInt() ) { gc = value.GetInt(); }
    else if ( value.IsUint() ) { gc = value.GetUint(); }
    else if ( value.IsInt64() ) { gc = value.GetInt64(); }
    else if ( value.IsUint64() ) { gc = value.GetUint64(); }
    else if ( value.IsDouble() ) { gc = value.GetDouble(); }
    else if ( value.IsString() ) { gc = value.GetString(); }
    else if ( value.IsArray() )
    {
      vector_type & V{ gc.set_vector( value.Size() ) };
      for ( rapidjson::SizeType i{ 0 }; i < value.Size(); ++i ) decode_json( V[i], value[i] );
    }
    else if ( value.IsObject() )
    {
      map_type &                         M{ gc.set_map() };
      map<std::string, GenericContainer> object;
      for ( auto it = value.MemberBegin(); ok && it != value.MemberEnd(); ++it )
      {
        GenericContainer & gc1 = M[it->name.GetString()];
        ok                     = decode_json( gc1, it->value );
      }
    }
    else
    {
      ok = false;
      std::cerr << "\n\n\n\n\n\nERRORE\n\n\n\n\n\n";
    }
    return ok;
  }

  bool GenericContainer::from_json( istream_type & stream )
  {
    // Usare un IStreamWrapper per leggere il file in un oggetto RapidJSON
    rapidjson::IStreamWrapper isw( stream );

    // Creare un oggetto Document per contenere il JSON
    rapidjson::Document document;

    // Parse del file
    document.ParseStream( isw );

    // Controllare eventuali errori di parsing
    if ( document.HasParseError() )
    {
      std::cerr << "Errore di parsing in JSON_to_GC\n";
      return false;
    }

    // rapidjson::StringBuffer sb;
    // rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    // document.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    // puts(sb.GetString());

    bool ok = decode_json( *this, document );
    if ( ok ) this->collapse();
    return ok;
  }

}  // namespace GC_namespace
