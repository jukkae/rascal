# Compile for macOs

CXX=clang
CPPFLAGS=-Wall -std=c++1z -framework SFML -framework sfml-window -framework sfml-graphics -framework sfml-system
INCLUDEDIRS=-I./include
LDLIBS=-L./lib
LDFLAGS=-ltcod -ltcodxx -lc++ -lboost_serialization

# g++ -framework sfml-window -framework sfml-graphics -framework sfml-system main.cpp -o main
# https://stackoverflow.com/questions/9054987/how-can-i-compile-sfml-project-via-command-line-on-mac
# maybe also -framework OpenGL -framework SFML

SOURCES=$(wildcard src/*.cpp)
OBJS=$(SRCS:.cpp=.o)
.PHONY: clean run

compile: clean $(OBJS)
	$(CXX) $(SOURCES) -o rascal $(CPPFLAGS) $(INCLUDEDIRS) -Iinclude $(LDLIBS) $(LDFLAGS) -I/usr/local/include

compile-debug: clean $(OBJS)
	$(CXX) $(SOURCES) -o rascal $(CPPFLAGS) -Iinclude $(LDLIBS) $(LDFLAGS) -I/usr/local/include -g -O0

clean:
	rm -rf *.o rascal save.txt rascal.dSYM

run:
	./rascal
