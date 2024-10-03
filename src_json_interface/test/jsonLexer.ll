%option c++ noyywrap yylineno
%option yyclass="jsonLexer"

%{
  #include "jsonLexer.hh"
  #include "jsonParser.tab.hh"

  // Definisci yylval come puntatore al tipo semantico del parser
  yy::parser::semantic_type yylval;
  yy::parser::location_type yylloc;

  #undef  YY_DECL
  #define YY_DECL int jsonLexer::yylex( yy::parser::semantic_type * yylval, yy::parser::location_type * yylloc )

  #define EAT_TOKEN(NAME) yylloc->step(); yylloc->columns(yyleng); *yylval = std::string(yytext, yyleng); return yy::parser::token::NAME;

%}


INT   [+-]?[0-9]+
DPART \.[0-9]+
EXP   [eE][+-]?[0-9]+

STRING \"([^"\\]|\\.)*\"
STRO   \"([^"\\]|\\[^\n])*?\"

%%

"{"               { EAT_TOKEN(LEFT_CURLY); }
"}"               { EAT_TOKEN(RIGHT_CURLY); }
"["               { EAT_TOKEN(LEFT_BRACE); }
"]"               { EAT_TOKEN(RIGHT_BRACE); }
","               { EAT_TOKEN(COMMA); }
":"               { EAT_TOKEN(COLON); }

"true"            { EAT_TOKEN(VTRUE); }
"false"           { EAT_TOKEN(VFALSE); }
"null"            { EAT_TOKEN(VNULL); }

[ \t\r\n]+        { yylloc->step(); yylloc->columns(yyleng); yylloc->lines(std::count(yytext, yytext + yyleng, '\n')); /* eat whitespace */ }

{STRO}            { yylloc->step(); yylloc->columns(yyleng);
                    *yylval = std::string(yytext+1, yyleng-2); // rimuove " e "
                    return yy::parser::token::STRING; }

{INT}{DPART}?{EXP}   { EAT_TOKEN(FLOAT); }
{INT}                { EAT_TOKEN(INTEGER); }

  /* Pass charater to bison which issue an error if wrong */

.                 { return *yytext; }

%%

// Funzione yyerror per stampare eventuali errori

void yyerror(const char* s) {
  fprintf(stderr, "Errore di parsing: %s\n", s);
}
