#include "jsonLexer.hh"
#include "jsonParser.tab.hh"

#include <sstream>
#include <cctype>
#include <iostream>

namespace yy {

#if 0
  bool
  jsonLexer::get_next_utf8_char() {
    m_last_utf8_char = "";
    unsigned char c;
    if ( !m_input->get(reinterpret_cast<char&>(c)) ) return false; // Stream ended or error

    unsigned len{0};
    if      ( (c & 0x80) == 0x00 ) len = 1; // 1 byte
    else if ( (c & 0xE0) == 0xC0 ) len = 2; // 2 bytes
    else if ( (c & 0xF0) == 0xE0 ) len = 3; // 3 bytes
    else if ( (c & 0xF8) == 0xF0 ) len = 4; // 4 bytes

    if ( len == 0 ) return false;  // Invalid UTF-8 start byte

    //string utf8Char(1, c); // Start with the first byte
    m_last_utf8_char = c;
    switch ( len ) {
    case 4:
      if ( !m_input->get(reinterpret_cast<char&>(c)) ) return false; // Stream ended or error
      if ( (c & 0xC0) != 0x80 ) return false; // Invalid UTF-8 continuation byte
      m_last_utf8_char += c; // Append continuation byte
    case 3:
      if ( !m_input->get(reinterpret_cast<char&>(c)) ) return false; // Stream ended or error
      if ( (c & 0xC0) != 0x80 ) return false; // Invalid UTF-8 continuation byte
      m_last_utf8_char += c; // Append continuation byte
    case 2:
      if ( !m_input->get(reinterpret_cast<char&>(c)) ) return false; // Stream ended or error
      if ( (c & 0xC0) != 0x80 ) return false; // Invalid UTF-8 continuation byte
      m_last_utf8_char += c; // Append continuation byte
    }
    return true;
  }
#endif


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

    char ch;
    // skip whitespace
    while ( m_input->get(ch) ) {
      if      ( ch == '\n'  ) { ++m_lineno; m_colno = 1; }
      else if ( isspace(ch) ) ++m_colno;
      else                    break;
    }

    if ( (reinterpret_cast<unsigned char&>(ch) & 0x80) != 0x00 ) return -1;

    yylloc->begin.line   = m_lineno;
    yylloc->begin.column = m_colno;

    std::istream::pos_type start_pos{m_input->tellg()};  // Salva la posizione corrente
    m_lexeme = ch;

    //std::cerr << "CHAR: " << c << '\n';

    if ( !m_input ) return 0; // EOF

    auto res{yy::parser::token::ERROR};

    switch (ch) {
    case '{': res = yy::parser::token::LEFT_CURLY;  break;
    case '}': res = yy::parser::token::RIGHT_CURLY; break;
    case '[': res = yy::parser::token::LEFT_BRACE;  break;
    case ']': res = yy::parser::token::RIGHT_BRACE; break;
    case ',': res = yy::parser::token::COMMA;       break;
    case ':': res = yy::parser::token::COLON;       break;
    case 't':  // true
      m_input->get(ch); m_lexeme += ch;
      m_input->get(ch); m_lexeme += ch;
      m_input->get(ch); m_lexeme += ch;
      if ( m_lexeme == "true" ) res = yy::parser::token::VTRUE;
      break;
    case 'f':  // false
      m_input->get(ch); m_lexeme += ch;
      m_input->get(ch); m_lexeme += ch;
      m_input->get(ch); m_lexeme += ch;
      m_input->get(ch); m_lexeme += ch;
      if ( m_lexeme == "false" ) res = yy::parser::token::VFALSE;
      break;
    case 'n':  // null
      m_input->get(ch); m_lexeme += ch;
      m_input->get(ch); m_lexeme += ch;
      m_input->get(ch); m_lexeme += ch;
      if ( m_lexeme == "null" ) res = yy::parser::token::VNULL;
      break;
    case '"':  // Stringa
      m_lexeme = ""; // reset string
      while ( m_input->get(ch) ) {
        if ( ch == '"' ) { m_colno += 2; res = yy::parser::token::STRING; break; }
        // controllo che non sia unicode
        if ( unicode_1(ch) ) {
          if ( ch == '\\' ) {
            if ( !m_input->get(ch) ) break; // fallita lettura
            switch (ch) {
            case 'n':  m_lexeme += '\n'; ++m_colno; break;
            case 't':  m_lexeme += '\t'; ++m_colno; break;
            case '\\': m_lexeme += '\\'; ++m_colno; break;
            case '"':  m_lexeme += '"';  ++m_colno; break;
            case 'r':  m_lexeme += '\r'; ++m_colno; break;
            case 'b':  m_lexeme += '\b'; ++m_colno; break;
            case 'f':  m_lexeme += '\f'; ++m_colno; break;
            default:
              m_lexeme += '\\';
              m_lexeme += ch;
            }
          } else {
            m_lexeme += ch;
          }
        } else {
          m_lexeme += ch;
          char ch1;
          if ( !m_input->get(ch1) ) break;
          if ( !ok_unicode(ch1)   ) break; // Invalid UTF-8 continuation byte
          m_lexeme += ch1;
          if ( unicode_min_3(ch) ) {
            if ( !m_input->get(ch1) ) break;
            if ( !ok_unicode(ch1)   ) break; // Invalid UTF-8 continuation byte
            m_lexeme += ch1;
            if ( unicode_min_4(ch) ) {
              if ( !m_input->get(ch1) ) break;
              if ( !ok_unicode(ch1)   ) break; // Invalid UTF-8 continuation byte
              m_lexeme += ch1;
            }
          }
        }
      }
      break;
    default:  // Numeri o altri caratteri
      int state{isdigit(ch)?1:0};
      if ( state == 1 || ch == '+' || ch == '-' ) {
        while ( m_input->get(ch) ) {
          if ( isdigit(ch) ) {
            if ( state == 0 ) state = 1;
            if ( state == 3 ) state = 4;
            m_lexeme += ch;
          } else if ( ch == '.' ) {
            if ( state > 1 ) break;
            state   = 2;
            m_lexeme += ch;
          } else if ( ch == '+' || ch == '-' ) {
            if ( state != 3 ) return false;
            state = 4;
            m_lexeme += ch;
          } else if ( ch == 'e' || ch == 'E' ) {
            if ( state > 2 || state == 0 ) break;
            state = 3;
            m_lexeme += ch;
          } else {
            //std::cout << "STATE = " << state << '\n';
            if ( state == 1 ) {
              m_input->putback(ch);
              res = yy::parser::token::INTEGER;
            }
            if ( state == 4 || state == 2 ) {
              m_input->putback(ch);
              res = yy::parser::token::FLOAT;
            }
            break;
          }
        }
      }
      break;
    }
    if ( res == yy::parser::token::ERROR ) {
      //std::cerr << "lexeme: " << m_lexeme << "\n";
      //std::cerr << "FAILED\n";
      m_input->seekg(start_pos);  // Ripristina la posizione iniziale
      return -1;  // Se nessuna corrispondenza
    } else {
      m_colno += m_lexeme.length();
      //yylloc->begin.line   = m_lineno;
      yylloc->begin.column = m_colno;
      (*yylval) = m_lexeme;
      //std::cerr << "TOKEN: " << m_lineno << ":" << m_colno << " " << m_lexeme << " TYPE: " << res << '\n';
    }
    return res;
  }
}
