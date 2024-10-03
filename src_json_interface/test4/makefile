# https://stackoverflow.com/questions/76509844/how-do-you-interface-c-flex-with-c-bison

.RECIPEPREFIX = >

prog: main.o jsonParser.o
	g++ -O2 -Wall -I../../include -std=c++11 $^ ../../src/*.cc -o $@

main.o: main.cc jsonParser.hh
	g++ -O2 -Wall -I../../include -std=c++11 -c $< -o $@

jsonParser.o: jsonParser.cc jsonParser.hh
	g++ -O2 -Wall -I../../include -std=c++11 -c $< -o $@

.PHONY: clean

clean:
	rm -f prog *.o

