# Compile for macOs

CPPFLAGS=-Wall -Wno-c++11-extensions
.PHONY: clean run

all:
	rm -f save.txt
	gcc src/*.cpp -o rascal $(CPPFLAGS) -Iinclude -L. -ltcod -ltcodxx -lc++ -I/usr/local/include -lboost_serialization

compile-debug:
	clean
	gcc src/*.cpp -o rascal $(CPPFLAGS) -Iinclude -L. -ltcod -ltcodxx -lc++ -I/usr/local/include -lboost_serialization -g -O0

clean:
	rm -f *.o rascal save.txt
	rm -f rascal.dSYM

run:
	./rascal
