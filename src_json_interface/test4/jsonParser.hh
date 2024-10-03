#pragma once

#ifndef JSON_PARSER_HH
#define JSON_PARSER_HH

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wpadded"
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif


#include <string>
#include <cctype>
#include <iostream>

#include "GenericContainer/GenericContainer.hh"

namespace GC_namespace {

  using std::istream;

  // -------------------------------------------------------
  class GenericContainerWrapper {
  private:

    GenericContainer*         m_data{nullptr};
    vector<GenericContainer*> m_stack;

  public:

    GenericContainerWrapper( GenericContainer * gc ) : m_data(gc) { m_stack.push_back(m_data); }
    ~GenericContainerWrapper() {}

    GenericContainer       & head()       { return *m_stack.back(); }
    GenericContainer const & head() const { return *m_stack.back(); }

    GenericContainer *
    get_gc() {
      GenericContainer * gc{m_stack.back()};
      if ( gc->get_type() == GC_namespace::GC_type::VECTOR ) {
        unsigned n{gc->get_num_elements()};
        gc = &(*gc)[n];
      }
      return gc;
    }

    void set_string( string const & s ) { get_gc()->set_string(s); }
    void set_float( string const & s ) {
      GenericContainer * gc{get_gc()};
      char * end{nullptr};
      double d = std::strtod( s.c_str(), &end );
      if ( *end == '\0' ) gc->set_real( d );
    }
    void set_integer( string const & s ) {
      GenericContainer * gc{get_gc()};
      char * end{nullptr};
      long long res = std::strtoll(s.c_str(), &end, 10);
      if ( *end == '\0' ) {
        if ( res <= std::numeric_limits<GC::int_type>::max() &&
             res >= std::numeric_limits<GC::int_type>::min() ) {
          gc->set_int(GC::int_type(res));
        } else {
          gc->set_long(res);
        }
      }
    }
    void set_true()  { get_gc()->set_bool(true); }
    void set_false() { get_gc()->set_bool(false); }
    void set_null()  { get_gc()->clear(); }

    void open_map()  { m_stack.push_back( get_gc() ); m_stack.back()->set_map(); }
    void close_map() { m_stack.pop_back(); }

    void open_vector()  { get_gc()->set_vector(); }
    void close_vector() { }

    void set_key( string const & s ) { m_stack.push_back( &( (*get_gc())[s] ) ); }
    void pop() { m_stack.pop_back(); }
  };


  class JSON_Parser {

    enum States : unsigned { START };

    enum Token : unsigned { ERROR=0, L_CURLY, R_CURLY, L_BRACE, R_BRACE, COMMA, COLON, VTRUE, VFALSE, VNULL, STRING, INTEGER, FLOAT };

    string        m_token;
    string        m_last_char;
    unsigned      m_lineno{1};
    unsigned      m_colno{1};
    istringstream m_stream;

    bool     get_next_utf8_char();
    unsigned next_token();

  public:

    JSON_Parser() { m_token.reserve(1023); }

    bool run( istream * in, GenericContainerWrapper & gc );

  };

}

#endif
