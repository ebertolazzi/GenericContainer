#include <iostream>
#include <fstream>
#include <chrono>
#include "jsonLexer.hh"
#include "jsonParser.tab.hh"


int
main(int argc, char *argv[]) {
    // Verifica che un file sia stato passato come argomento
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.json>" << std::endl;
        return 1;
    }

    // Apri il file
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }

    // Esegui il parsing
    auto start = std::chrono::high_resolution_clock::now();

    // Crea il lexer e reindirizza l'input verso il file
    yy::jsonLexer lexer;  // Reindirizza lo stream del lexer al file
    lexer.set_stream(&file);

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "SET STREAM " << duration.count() << " millisecondi." << std::endl;

    start = std::chrono::high_resolution_clock::now();

    // Crea il parser e passa il lexer
    GenericContainer gc;
    GenericContainerWrapper gcw(&gc);
    yy::parser parser(lexer,&gcw);

    int result = parser();

    end      = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "PARSE " << duration.count() << " millisecondi." << std::endl;

    // Chiudi il file
    file.close();

    if ( result == 0 && false ) {
      std::cout << "\n\nPRINT GC\n\n";
      std::cout.precision(10);
      gc.print(std::cout);
    }

    return result;
}
