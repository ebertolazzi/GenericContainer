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
#endif

#include "GenericContainerJsonHandler.hh"

#include <fstream>
#include <string>
#include <iomanip>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "rapidjson/prettywriter.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

using namespace GC;
using namespace rapidjson;
using namespace std;

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

  string const GC_JSON_IM_UNIT     {"imaginary_unit"};
  string const GC_JSON_MAT_ORDER   {"matrix_ordering"};
  string const GC_JSON_PRETTY      {"prettify"};
  string const GC_JSON_INDENT_CHAR {"indent_char"};
  string const GC_JSON_INDENT_NUM  {"indent_num"};

  int_type const GC_JSON_column_major = 0;
  int_type const GC_JSON_row_major    = 1;
  int_type const GC_JSON_none         = 2;

  // Funzione ricorsiva per convertire un JSON in un GenericContainer
  static
  bool
  parse_json( GenericContainer & gc, rapidjson::Value const & value ) {
    bool ok{true};
    if ( value.IsBool() ) {
      gc = value.GetBool();
    } else if ( value.IsInt() ) {
      gc = value.GetInt();
    } else if ( value.IsDouble() ) {
      gc = value.GetDouble();
    } else if ( value.IsString() ) {
      gc = value.GetString();
    } else if ( value.IsArray() ) {
      vector_type & V = gc.set_vector( value.Size() );
      for ( rapidjson::SizeType i{0}; i < value.Size(); ++i )
        parse_json( V[i], value[i] );
    } else if ( value.IsObject() ) {
      map_type & M = gc.set_map();
      map<std::string, GenericContainer> object;
      for ( auto it = value.MemberBegin(); ok && it != value.MemberEnd(); ++it ) {
        GenericContainer & gc1 = M[it->name.GetString()];
        ok = parse_json(gc1,it->value);
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

    bool ok = parse_json(gc,document);
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
    bool ok = parse_json(gc,document);
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
