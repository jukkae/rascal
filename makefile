# Compile for macOs

CXX=clang
CPPFLAGS=-Wall -std=c++1z
INCLUDEDIRS=-I./include
LDFLAGS=-lc++ -lboost_serialization -framework SFML -framework sfml-window -framework sfml-graphics -framework sfml-system

SRC := src
OBJ := obj

# g++ -framework sfml-window -framework sfml-graphics -framework sfml-system main.cpp -o main
# https://stackoverflow.com/questions/9054987/how-can-i-compile-sfml-project-via-command-line-on-mac
# maybe also -framework OpenGL -framework SFML

SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))
.PHONY: clean run

rascal: $(OBJECTS)
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) -I$(SRC) -c $< -o $@ $(CPPFLAGS)

#compile: clean $(OBJS)
	#$(CXX) $(SOURCES) -o rascal $(CPPFLAGS) $(INCLUDEDIRS) -Iinclude $(LDFLAGS) -I/usr/local/include

#compile-debug: clean $(OBJS)
	#$(CXX) $(SOURCES) -o rascal $(CPPFLAGS) -Iinclude $(LDFLAGS) -I/usr/local/include -g -O0

clean:
	rm -rf *.o rascal save.txt rascal.dSYM

run:
	./rascal
