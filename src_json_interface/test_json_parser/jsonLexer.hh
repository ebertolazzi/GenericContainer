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

#include "location.hh"

#include <string>
#include <cctype>
#include <sstream>

namespace yy
{
  class jsonLexer
  {
  public:
    jsonLexer() { m_lexeme.reserve( 1023 ); }
    int yylex( std::string * yylval, yy::location * yylloc );

    void set_stream( std::istream * file )
    {
      // Utilizziamo un buffer di stringa per leggere tutto il contenuto dello stream
      std::ostringstream oss;
      oss << file->rdbuf();  // Leggiamo tutto il contenuto nel buffer della stringa
      m_input  = oss.str();
      m_cursor = m_input.begin();
    }

  private:
    std::string                 m_lexeme;
    std::istringstream          m_input_local;
    std::string                 m_input;
    std::string::const_iterator m_cursor;
    int                         m_lineno{ 1 };
    int                         m_colno{ 1 };
  };
}  // namespace yy

#endif
