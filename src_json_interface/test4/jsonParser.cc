#include "jsonParser.hh"


namespace GC_namespace {

  bool
  JSON_Parser::get_next_utf8_char() {
    m_last_char = "";
    unsigned char c;
    if ( !m_stream.get(reinterpret_cast<char&>(c)) ) return false; // Stream ended or error

    unsigned len{0};
    if      ( (c & 0x80) == 0x00 ) len = 1; // 1 byte
    else if ( (c & 0xE0) == 0xC0 ) len = 2; // 2 bytes
    else if ( (c & 0xF0) == 0xE0 ) len = 3; // 3 bytes
    else if ( (c & 0xF8) == 0xF0 ) len = 4; // 4 bytes

    if ( len == 0 ) return false;  // Invalid UTF-8 start byte

    string utf8Char(1, c); // Start with the first byte
    for ( unsigned i = 1; i < len; ++i) {
      if ( !m_stream.get(reinterpret_cast<char&>(c)) ) return false; // Stream ended or error
      // Check if the byte is a valid continuation byte
      if ( (c & 0xC0) != 0x80 ) return false; // Invalid UTF-8 continuation byte
      utf8Char += c; // Append continuation byte
    }
    m_last_char = utf8Char;
    return true;
  }

  unsigned
  JSON_Parser::next_token() {

    // skip whitespace
    while ( get_next_utf8_char() ) {
      if      ( m_last_char[0] == '\n'  ) { ++m_lineno; m_colno = 1; }
      else if ( isspace(m_last_char[0]) ) ++m_colno;
      else                                break;
    }

    std::istream::pos_type start_pos{m_stream.tellg()}; // Salva la posizione corrente
    m_token = m_last_char;

    if ( !m_stream ) return 0; // EOF

    unsigned res{ERROR};
    unsigned state{1};

    switch (m_last_char[0]) {
    case '{': res = L_CURLY; break;
    case '}': res = R_CURLY; break;
    case '[': res = L_BRACE; break;
    case ']': res = R_BRACE; break;
    case ',': res = COMMA;   break;
    case ':': res = COLON;   break;
    case 't':
      get_next_utf8_char(); m_token += m_last_char;
      get_next_utf8_char(); m_token += m_last_char;
      get_next_utf8_char(); m_token += m_last_char;
      if ( m_token == "true" ) res = VTRUE;
      break;
    case 'f':  // false
      get_next_utf8_char(); m_token += m_last_char;
      get_next_utf8_char(); m_token += m_last_char;
      get_next_utf8_char(); m_token += m_last_char;
      get_next_utf8_char(); m_token += m_last_char;
      if ( m_token == "false" ) res = VFALSE;
      break;
    case 'n':  // null
      get_next_utf8_char(); m_token += m_last_char;
      get_next_utf8_char(); m_token += m_last_char;
      get_next_utf8_char(); m_token += m_last_char;
      if ( m_token == "null" ) res = VNULL;
      break;
    case '"':  // Stringa
      m_token = "";
      while ( get_next_utf8_char() ) {
        if ( m_last_char[0] == '"'  ) { m_colno += 2; res = STRING; break; }
        ++m_colno;
        if ( m_last_char[0] == '\\' ) {
          if ( !get_next_utf8_char() ) break;
          switch (m_last_char[0]) {
          case 'n':  m_token += '\n'; break;
          case 't':  m_token += '\t'; break;
          case '"':  m_token += '"';  break;
          case 'r':  m_token += '\r'; break;
          case 'b':  m_token += '\b'; break;
          case 'f':  m_token += '\f'; break;
          case '\\': m_token += '\\'; break;
          default:
            m_token += '\\';
            m_token += m_last_char;
            ++m_colno;
          }
        } else {
          m_token += m_last_char;
        }
      }
      break;
    case '+':
    case '-':
      state = 0;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      std::istream::pos_type ipos{start_pos}; // Salva la posizione corrente
      while ( get_next_utf8_char() ) {
        if ( isdigit(m_last_char[0]) ) {
          if ( state == 0 ) state = 1;
          if ( state == 3 ) state = 4;
          m_token += m_last_char;
        } else if ( m_last_char[0] == '.' ) {
          if ( state > 1 ) break;
          state = 2;
          m_token += m_last_char;
        } else if ( m_last_char[0] == '+' || m_last_char[0] == '-' ) {
          if ( state != 3 ) return false;
          state = 4;
          m_token += m_last_char;
        } else if ( m_last_char[0] == 'e' || m_last_char[0] == 'E' ) {
          if ( state > 2 || state == 0 ) break;
          state = 3;
          m_token += m_last_char;
        } else {
          //std::cout << "STATE = " << state << '\n';
          if ( state == 1               ) res = INTEGER;
          if ( state == 4 || state == 2 ) res = FLOAT;
          m_stream.seekg(ipos);  // Ripristina ultima posizione carattere numero
          break;
        }
        ipos = m_stream.tellg(); // Salva la posizione corrente
      }
      break;
    }
    return res;
  }


  bool
  JSON_Parser::run( istream * in, GenericContainerWrapper & gcw ) {

    // Salviamo lo stato del buffer corrente del flusso
    std::istream::sentry guard(*in, true);

    // Verifichiamo che lo stream sia valido
    if (!guard || !(*in)) {
        std::cerr << "Errore nel flusso di input!" << std::endl;
        return "";
    }

    // Utilizziamo un buffer di stringa per leggere tutto il contenuto dello stream
    std::ostringstream oss;
    oss << in->rdbuf(); // Leggiamo tutto il contenuto nel buffer della stringa

    std::cout << oss.str().length() << "\n"; // Restituiamo il contenuto come una stringa

    unsigned tk;
    m_stream.str(oss.str());
    while ( (tk=next_token()) != ERROR ) {
      //std::cerr << "TOKEN: " << m_token << '\n';
    }
    return true;
  }

}
