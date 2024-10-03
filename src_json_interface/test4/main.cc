#include <iostream>
#include <fstream>
#include <chrono>
#include "jsonParser.hh"


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

  // Crea il parser e passa il lexer
  GC_namespace::GenericContainer        gc;
  GC_namespace::GenericContainerWrapper gcw(&gc);

  // Crea il lexer e reindirizza l'input verso il file
  GC_namespace::JSON_Parser parser;  // Reindirizza lo stream del lexer al file

  // Esegui il parsing
  auto start = std::chrono::high_resolution_clock::now();
  bool ok = parser.run(&file,gcw);  // Reindirizza lo stream del lexer al file
  auto end      = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PARSE " << duration.count() << " millisecondi." << std::endl;

  // Chiudi il file
  file.close();

  if ( false ) {
    std::cout << "\n\nPRINT GC\n\n";
    std::cout.precision(10);
    gc.print(std::cout);
  }

  return 0;
}
