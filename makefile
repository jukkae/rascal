# Compile for macOs

CXX=gcc
CPPFLAGS=-Wall -std=c++1z
LDLIBS=-L./lib
LDFLAGS=-ltcod -ltcodxx -lc++ -lboost_serialization

SOURCES=$(wildcard src/*.cpp)
OBJS=$(SRCS:.cpp=.o)
.PHONY: clean run

compile: clean $(OBJS)
	$(CXX) $(SOURCES) -o rascal $(CPPFLAGS) -Iinclude $(LDLIBS) $(LDFLAGS) -I/usr/local/include

compile-debug: clean $(OBJS)
	$(CXX) $(SOURCES) -o rascal $(CPPFLAGS) -Iinclude $(LDLIBS) $(LDFLAGS) -I/usr/local/include -g -O0

clean:
	rm -rf *.o rascal save.txt rascal.dSYM

run:
	./rascal
