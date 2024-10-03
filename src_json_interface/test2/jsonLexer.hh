#ifndef JSONLEXER_HH
#define JSONLEXER_HH

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


// Includi il file di intestazione del parser generato da Bison
#include "location.hh"

#include <string>
#include <cctype>

namespace yy {
  class jsonLexer {
  public:
    jsonLexer( std::istringstream * in ) : m_input(in) {}
    int yylex( std::string * yylval, yy::location *yylloc );

  private:
    std::istringstream * m_input;
    int                  m_lineno{1};
    int                  m_colno{1};
  };
}

#endif
