#include "jsonLexer.hh"
#include "jsonParser.tab.hh"

#include <sstream>
#include <cctype>
#include <iostream>

namespace yy {

  inline bool unicode_1( char c ) { return ( reinterpret_cast<unsigned char&>(c) & 0x80) == 0x00; }
  inline bool unicode_2( char c ) { return ( reinterpret_cast<unsigned char&>(c) & 0xE0) == 0xC0; }
  inline bool unicode_3( char c ) { return ( reinterpret_cast<unsigned char&>(c) & 0xF0) == 0xE0; }
  inline bool unicode_4( char c ) { return ( reinterpret_cast<unsigned char&>(c) & 0xF8) == 0xF0; }

  inline bool unicode_min_2( char c ) { return ( reinterpret_cast<unsigned char&>(c) & 0xC0 ) == 0xC0; }
  inline bool unicode_min_3( char c ) { return ( reinterpret_cast<unsigned char&>(c) & 0xE0 ) == 0xE0; }
  inline bool unicode_min_4( char c ) { return ( reinterpret_cast<unsigned char&>(c) & 0xF0 ) == 0xF0; }

  inline bool ok_unicode ( char c ) { return ( reinterpret_cast<unsigned char&>(c) & 0xC0 ) == 0x80; }

  // Restituisce il prossimo token
  int
  jsonLexer::yylex( std::string * yylval, yy::location * yylloc ) {

    if ( m_cursor == m_input.end() ) return 0; // EOF

    // skip whitespace
    do {
      char const & ch{*m_cursor};
      if      ( ch == '\n'  ) { ++m_lineno; m_colno = 1; }
      else if ( isspace(ch) ) ++m_colno;
      else                    break;
    } while ( ++m_cursor != m_input.end() );

    auto res{yy::parser::token::ERROR};

    if ( !unicode_1(*m_cursor) ) return res;

    yylloc->begin.line   = m_lineno;
    yylloc->begin.column = m_colno;

    std::string::const_iterator start_pos{m_cursor};  // Salva la posizione corrente
    m_lexeme = *m_cursor;

    //std::cerr << "CHAR: " << *m_cursor << '\n';

    switch (*m_cursor) {
    case '{': res = yy::parser::token::LEFT_CURLY;  ++m_cursor; break;
    case '}': res = yy::parser::token::RIGHT_CURLY; ++m_cursor; break;
    case '[': res = yy::parser::token::LEFT_BRACE;  ++m_cursor; break;
    case ']': res = yy::parser::token::RIGHT_BRACE; ++m_cursor; break;
    case ',': res = yy::parser::token::COMMA;       ++m_cursor; break;
    case ':': res = yy::parser::token::COLON;       ++m_cursor; break;
    case 't':  // true
      if ( std::strncmp("true",&*m_cursor,4) == 0 )
        { m_lexeme = "true"; m_cursor += 4; res = yy::parser::token::VTRUE; }
      break;
    case 'f':  // false
      if ( std::strncmp("false",&*m_cursor,5) == 0 )
        { m_lexeme = "false"; m_cursor += 5; res = yy::parser::token::VFALSE; }
      break;
    case 'n':  // null
      if ( std::strncmp("null",&*m_cursor,4) == 0 )
        { m_lexeme = "null"; m_cursor += 4; res = yy::parser::token::VNULL; }
      break;
    case '"':  // Stringa
      m_lexeme = ""; // reset string
      while ( ++m_cursor != m_input.end() ) {
        char const & ch0{*m_cursor};
        if ( ch0 == '"' ) { ++m_cursor; res = yy::parser::token::STRING; break; }
        // controllo che non sia unicode
        if ( unicode_1(ch0) ) {
          if ( ch0 == '\\' ) {
            if ( ++m_cursor == m_input.end() ) return 0; // EOF
            char const & ch1{*m_cursor};
            switch (ch1) {
            case 'n':  m_lexeme += '\n'; break;
            case 't':  m_lexeme += '\t'; break;
            case '\\': m_lexeme += '\\'; break;
            case '"':  m_lexeme += '"';  break;
            case 'r':  m_lexeme += '\r'; break;
            case 'b':  m_lexeme += '\b'; break;
            case 'f':  m_lexeme += '\f'; break;
            default:
              m_lexeme += '\\';
              m_lexeme += ch1;
            }
          } else {
            m_lexeme += ch0;
          }
        } else { // multibyte unicode
          m_lexeme += ch0;
          if ( ++m_cursor == m_input.end() ) return 0; // EOF
          char const & ch1{*m_cursor};
          if ( !ok_unicode(ch1) ) break; // Invalid UTF-8 continuation byte
          m_lexeme += ch1;
          if ( unicode_min_3(ch0) ) {
            if ( ++m_cursor == m_input.end() ) return 0; // EOF
            char const & ch2{*m_cursor};
            if ( !ok_unicode(ch2) ) break; // Invalid UTF-8 continuation byte
            m_lexeme += ch2;
            if ( unicode_min_4(ch0) ) {
              if ( ++m_cursor == m_input.end() ) return 0; // EOF
              char const & ch3{*m_cursor};
              if ( !ok_unicode(ch3) ) break; // Invalid UTF-8 continuation byte
              m_lexeme += ch3;
            }
          }
        }
      }
      break;
    case '+':
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': // Numeri o altri caratteri
      char const & ch0{*m_cursor};
      int state{ (ch0 == '+' || ch0 == '-') ? 0 : 1 };
      while ( ++m_cursor != m_input.end() ) {
        char const & ch{*m_cursor};
        m_lexeme += ch;
        if ( isdigit(ch) ) {
          if      ( state == 0 ) state = 1;
          else if ( state == 3 ) state = 4;
        } else if ( ch == '.' ) {
          if ( state > 1 ) break;
          state = 2;
        } else if ( ch == 'e' || ch == 'E' ) {
          if ( state > 2 || state == 0 ) break;
          state = 3;
        } else if ( ch == '+' || ch == '-' ) {
          if ( state != 3 ) return false;
          state = 4;
        } else {
          //std::cout << "STATE = " << state << '\n';
          m_lexeme.pop_back();
          if      ( state == 1               ) res = yy::parser::token::INTEGER;
          else if ( state == 4 || state == 2 ) res = yy::parser::token::FLOAT;
          break;
        }
      }
      break;
    }
    if ( res == yy::parser::token::ERROR ) {
      //std::cerr << "lexeme: " << m_lexeme << "\n";
      //std::cerr << "FAILED\n";
      m_cursor = start_pos;  // Ripristina la posizione iniziale
      return -1;             // Se nessuna corrispondenza
    } else {
      m_colno += m_cursor-start_pos;
      //yylloc->begin.line   = m_lineno;
      yylloc->begin.column = m_colno;
      (*yylval) = m_lexeme;
      //std::cerr << "TOKEN: " << m_lineno << ":" << m_colno << " " << m_lexeme << " TYPE: " << res << '\n';
    }
    if ( m_cursor > m_input.end() ) {
      std::cerr << "ERRORE\n";
      std::exit(1);
    }
    return res;
  }
}
