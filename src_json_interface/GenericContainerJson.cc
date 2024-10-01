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

#include "GenericContainer/GenericContainerJsonInterface.hh"

#include <fstream>
#include <string>
#include <iomanip>
#include <regex>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "rapidjson/prettywriter.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#endif

/*
 The encoding of a GenericContainer to a Json is much simpler than the reverse operation, and it is
 done basically by the C-like function 'gc_to_writer'.
 The implementation of the header GenericContainerJson.hh then follows.
 */

namespace GC_namespace {

  using std::ifstream;
  using std::ofstream;
  using std::string;
  using std::regex;
  using std::smatch;

  static
  bool
  parse_complex( string const & input, complex_type & res ) {
    // Regular expression per catturare la parte reale e immaginaria
    std::regex complex_regex(R"(([+-]?(?:\d*\.\d+|\d+)(?:[eE][+-]?\d+)?)?([+-]?(?:\d*\.\d+|\d+)?)(?:\*?i)?)");

    // Match del primo numero
    std::smatch match;
    bool ok = std::regex_search(input, match, complex_regex);
    if ( ok ) {
      // Parte reale (match[1]) e parte immaginaria (match[2])
      real_type re(0), im(0);
      if ( match[1].matched ) { if ( !match[1].str().empty() ) re = stod(match[1].str()); }
      if ( match[2].matched ) { if ( !match[2].str().empty() ) im = stod(match[2].str()); }
      res = complex_type( re, im );
    }
    return ok;
  }

  // Funzione ricorsiva per convertire un JSON in un GenericContainer
  static
  bool
  decode_json( GenericContainer & gc, rapidjson::Value const & value ) {
    // Regex per riconoscere numeri complessi (es: "3.5+4.2i" o "-2.5-3.1i")
    static regex complex_regex(R"(^([+-]?\d+(\.\d+)?)[+-](\d+(\.\d+)?)i$)");

    bool ok{true};
    if ( value.IsNull() ) {
      gc.clear();
    } else if ( value.IsBool() ) {
      gc = value.GetBool();
    } else if ( value.IsInt() ) {
      gc = value.GetInt();
    } else if ( value.IsUint() ) {
      gc = value.GetUint();
    } else if ( value.IsInt64() ) {
      gc = value.GetInt64();
    } else if ( value.IsUint64() ) {
      gc = value.GetUint64();
    } else if ( value.IsDouble() ) {
      gc = value.GetDouble();
    } else if ( value.IsString() ) {
      string const & s{ value.GetString() };
      complex_type c;
      if ( parse_complex( s, c ) ) gc = c;
      else                         gc = s;
    } else if ( value.IsArray() ) {
      vector_type & V{ gc.set_vector( value.Size() ) };
      for ( rapidjson::SizeType i{0}; i < value.Size(); ++i )
        decode_json( V[i], value[i] );
    } else if ( value.IsObject() ) {
      map_type & M{ gc.set_map() };
      map<std::string, GenericContainer> object;
      for ( auto it = value.MemberBegin(); ok && it != value.MemberEnd(); ++it ) {
        GenericContainer & gc1 = M[it->name.GetString()];
        ok = decode_json(gc1,it->value);
      }
    } else {
      ok = false;
      std::cerr << "\n\n\n\n\n\nERRORE\n\n\n\n\n\n";
    }
    return ok;
  }

  bool
  JSON_to_GC(
    string const     & JSON,
    GenericContainer & gc
  ) {

    rapidjson::Document document;
    document.Parse(JSON.c_str());

    if (document.HasParseError()) {
      std::cerr << "Errore durante il parsing del JSON" << std::endl;
      return false;
    }

    bool ok{ decode_json(gc,document) };
    if ( ok ) gc.collapse();
    return ok;

  }

  bool
  JSON_to_GC(
    istream_type     & file_JSON,
    GenericContainer & gc
  ) {
    // Usare un IStreamWrapper per leggere il file in un oggetto RapidJSON
    rapidjson::IStreamWrapper isw(file_JSON);

    // Creare un oggetto Document per contenere il JSON
    rapidjson::Document document;

    // Parse del file
    document.ParseStream(isw);

    // Controllare eventuali errori di parsing
    if (document.HasParseError()) {
      std::cerr << "Errore di parsing in JSON_to_GC\n";
      return false;
    }
    bool ok = decode_json(gc,document);
    if ( ok ) gc.collapse();
    return ok;
  }

  bool
  file_JSON_to_GC(
    string const     & json_file_name,
    GenericContainer & gc
  ) {
    ifstream file_JSON(json_file_name);
    bool ok = JSON_to_GC( file_JSON, gc );
    file_JSON.close();
    return ok;
  }

}
