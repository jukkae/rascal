# Compile for macOs

CXX=gcc
CPPFLAGS=-Wall -Wno-c++11-extensions
LDLIBS=-L./lib
LDFLAGS=-ltcod -ltcodxx -lc++ -lboost_serialization

SRCS=src/*.cpp
# OBJS=$(subst .cpp,.o,$(SRCS))
.PHONY: clean run

all: clean
	$(CXX) $(SRCS) -o rascal $(CPPFLAGS) -Iinclude $(LDLIBS) $(LDFLAGS) -I/usr/local/include

compile-debug: clean
	$(CXX) $(SRCS) -o rascal $(CPPFLAGS) -Iinclude $(LDLIBS) $(LDFLAGS) -I/usr/local/include -g -O0

clean:
	rm -rf *.o rascal save.txt rascal.dSYM

run:
	./rascal
