#include "jsonLexer.hh"
#include "jsonParser.tab.hh"

#include <sstream>
#include <cctype>
#include <iostream>

// Restituisce il prossimo token
int
yy::jsonLexer::yylex( std::string * yylval, yy::location *yylloc ) {

  // skip whitespace
  char c;
  while (m_input->get(c)) {
    if      ( c == '\n'  ) { ++m_lineno; m_colno = 1; }
    else if ( isspace(c) ) ++m_colno;
    else                   break;
  }

  yylloc->begin.line   = m_lineno;
  yylloc->begin.column = m_colno;

  std::istringstream::pos_type start_pos{m_input->tellg()};  // Salva la posizione corrente
  std::string & lexeme{*yylval};
  lexeme = c;

  //std::cerr << "C=" << c << '\n';

  if (!m_input) {
    std::cout << "EOF\n";
    return 0;  // EOF
  }

  union {
    char     c[4];
    uint32_t i;
  } buffer, bT, bF, bN;

  bT.i = 0; bT.c[0] = 'r'; bT.c[1] = 'u'; bT.c[2] = 'e';
  bF.i = 0; bF.c[0] = 'a'; bF.c[1] = 'l'; bF.c[2] = 's'; bF.c[3] = 'e';
  bN.i = 0; bN.c[0] = 'u'; bN.c[1] = 'l'; bN.c[2] = 'l';

  auto res{yy::parser::token::ERROR};

  switch (c) {
  case '{': res = yy::parser::token::LEFT_CURLY;  break;
  case '}': res = yy::parser::token::RIGHT_CURLY; break;
  case '[': res = yy::parser::token::LEFT_BRACE;  break;
  case ']': res = yy::parser::token::RIGHT_BRACE; break;
  case ',': res = yy::parser::token::COMMA;       break;
  case ':': res = yy::parser::token::COLON;       break;
  case 't':  // true
    buffer.i = 0;
    m_input->get(buffer.c[0]);
    m_input->get(buffer.c[1]);
    m_input->get(buffer.c[2]);
    if ( m_input && buffer.i == bT.i ) { lexeme = "true"; res = yy::parser::token::VTRUE; }
    break;
  case 'f':  // false
    buffer.i = 0;
    m_input->get(buffer.c[0]);
    m_input->get(buffer.c[1]);
    m_input->get(buffer.c[2]);
    m_input->get(buffer.c[3]);
    if ( m_input && buffer.i == bF.i ) { lexeme = "false"; res = yy::parser::token::VFALSE; }
    break;
  case 'n':  // null
    buffer.i = 0;
    m_input->get(buffer.c[0]);
    m_input->get(buffer.c[1]);
    m_input->get(buffer.c[2]);
    if ( m_input && buffer.i == bN.i ) { lexeme = "null"; res = yy::parser::token::VNULL; }
    break;
  case '"':  // Stringa
    lexeme = "";
    while ( m_input->get(c) ) {
      if ( c == '"' ) { m_colno +=2; res = yy::parser::token::STRING; break; }
      lexeme += c;
      if ( c == '\\' ) {  // Gestione caratteri di escape
        if (m_input->get(c)) lexeme += c;
        else                 break;
      }
    }
    break;
  default:  // Numeri o altri caratteri
    if ( isdigit(c) || c == '+' || c == '-' ) {
      int state{isdigit(c)?1:0};
      lexeme += c;
      while ( m_input->get(c) ) {
        if ( isdigit(c) ) {
          if ( state == 0 ) state = 1;
          if ( state == 3 ) state = 4;
          lexeme += c;
        } else if ( c == '.' ) {
          if ( state > 1 ) break;
          state   = 2;
          lexeme += c;
        } else if ( c == '+' || c == '-' ) {
          if ( state != 3 ) return false;
          state = 4;
          lexeme += c;
        } else if ( c == 'e' || c == 'E' ) {
          if ( state > 2 || state == 0 ) break;
          state = 3;
          lexeme += c;
        } else {
          if ( state == 1 ) {
            m_input->putback(c);
            res = yy::parser::token::INTEGER;
          }
          if ( state == 4 ) {
            m_input->putback(c);
            res = yy::parser::token::FLOAT;
          }
          break;
        }
      }
    }
    break;
  }
  if ( res == yy::parser::token::ERROR ) {
    //std::cerr << "lexeme: " << lexeme << "\n";
    //std::cerr << "FAILED\n";
    m_input->seekg(start_pos);  // Ripristina la posizione iniziale
    return -1;  // Se nessuna corrispondenza
  } else {
    m_colno += lexeme.length();
    yylloc->begin.line   = m_lineno;
    yylloc->begin.column = m_colno;
    //std::cerr << "TOKEN: " << lineno << ":" << colno << " " << lexeme << " TYPE: " << res << '\n';
  }
  return res;
}
