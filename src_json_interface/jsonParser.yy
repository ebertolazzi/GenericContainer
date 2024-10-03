%require "3.8"
%language "c++"

%code requires {
  #include <cstdlib>
  #include "jsonLexer.hh"
  #include "GenericContainer.hh"

  using GC::GenericContainer;
  using GC::vector;
  using GC::string;

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

}

%define api.value.type {std::string}

%define parse.trace
%define parse.error detailed

%parse-param {jsonLexer & lexer}
%parse-param {GenericContainerWrapper * gcw}
%locations

%header

%code {
  #define yylex lexer.yylex
}

%token LEFT_CURLY RIGHT_CURLY LEFT_BRACE RIGHT_BRACE COMMA COLON
%token VTRUE VFALSE VNULL
%token STRING;
%token FLOAT INTEGER;
%token ERROR;

%start json

%%

json: /* empty */
    | value
    ;

value: object
     | array
     | STRING  { gcw->set_string($1);  }
     | INTEGER { gcw->set_integer($1); }
     | FLOAT   { gcw->set_float($1);   }
     | VTRUE   { gcw->set_true();      }
     | VFALSE  { gcw->set_false();     }
     | VNULL   { gcw->set_null();      }
     ;

object: LEFT_CURLY  { gcw->open_map(); }         RIGHT_CURLY { gcw->close_map(); }
      | LEFT_CURLY  { gcw->open_map(); } members RIGHT_CURLY { gcw->close_map(); }
      ;

members: member
       | members COMMA member
       ;

member: STRING { gcw->set_key($1); } COLON value { gcw->pop(); }
      ;

array: LEFT_BRACE  { gcw->open_vector(); }        RIGHT_BRACE { gcw->close_vector(); }
     | LEFT_BRACE  { gcw->open_vector(); } values RIGHT_BRACE { gcw->close_vector(); }
     ;

values: value
      | values COMMA value
      ;

%%

void
yy::parser::error(
  yy::parser::location_type const & location,
  std::string               const & message
) {
  std::cerr << "Error at line " << location.begin.line
            << ", column " << location.begin.column
            << ": " << message << std::endl;
}
