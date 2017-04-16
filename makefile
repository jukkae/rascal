# Trivial compile for osx
all:
	gcc src/*.cpp -o rascal -Iinclude -L. -ltcod -ltcodxx -Wall -lc++
