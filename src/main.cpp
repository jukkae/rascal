#include <fstream>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

#include "persistent.hpp"

#include "libtcod.hpp"
#include "engine.hpp"
#include "main.hpp"

Engine engine;

int main() {
	while(!TCODConsole::isWindowClosed()) {
		engine.update();
	}

	return 0;
}
