#include <fstream>
#include <string>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

//#include "jsonLexer.hh"
#include "jsonParser.tab.hh"

namespace GC_namespace {

  using std::ifstream;
  using std::ofstream;
  using std::string;

  bool
  GenericContainer::from_json2( istream_type & stream ) {

    // Crea il lexer e reindirizza l'input verso il file
    yy::jsonLexer lexer;
    lexer.set_stream(&stream);

    // Crea il parser e passa il lexer
    GenericContainerWrapper gcw(this);
    yy::parser parser(lexer,&gcw);

    // Esegui il parsing
    bool ok{parser() == 0};
    if ( ok ) this->collapse();
    return ok;
  }

}

#endif
