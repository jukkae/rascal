# Trivial compile for osx
all:
	rm -f save.txt
	gcc src/*.cpp -o rascal -Iinclude -L. -ltcod -ltcodxx -Wall -lc++ -I/usr/local/include -lboost_serialization

clean:
	rm -f rascal
	rm -f save.txt

run:
	./rascal
