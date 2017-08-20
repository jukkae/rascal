#include <fstream>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

#include "persistent.hpp"

#include "libtcod.hpp"
#include "engine.hpp"
#include "main.hpp"

std::string file = "save.txt";

Engine engine(1);

int main() {
	while(!TCODConsole::isWindowClosed()) {
		engine.update();
	}

	return 0;
}

void load() {
	std::ifstream ifs(file);
	boost::archive::text_iarchive ia(ifs);
	ia >> engine;
}

void save() {
	std::ofstream ofs(file);
	boost::archive::text_oarchive oa(ofs);
	oa << engine;
}
