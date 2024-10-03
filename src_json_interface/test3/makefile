# https://stackoverflow.com/questions/76509844/how-do-you-interface-c-flex-with-c-bison

.RECIPEPREFIX = >

prog: main.o jsonParser.tab.o jsonLexer.o
	g++ -O -Wall -I../../include -std=c++11 $^ ../../src/*.cc -o $@

main.o: main.cc jsonLexer.hh jsonParser.tab.hh
	g++ -O -Wall -I../../include -std=c++11 -c $< -o $@

jsonLexer.o: jsonLexer.cc jsonLexer.hh jsonParser.tab.hh
	g++ -O -Wall -I../../include -std=c++11 -c $< -o $@

jsonParser.tab.o: jsonParser.tab.cc jsonLexer.hh
	g++ -O -Wall -I../../include -std=c++11 -c $< -o $@

jsonParser.tab.hh jsonParser.tab.cc jsonParser.output: jsonParser.yy
	/usr/local/opt/bison/bin/bison -Wcounterexamples -Wconflicts-sr $<

.PHONY: clean
clean:
	rm -f prog main.o lex.* jsonParser.tab.* stack.hh location.hh

