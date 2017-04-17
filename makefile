# Trivial compile for osx
all:
	gcc src/*.cpp -o rascal -Iinclude -L. -ltcod -ltcodxx -Wall -lc++ -I/usr/local/include -lboost_serialization
