#include <fstream>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

#include "persistent.hpp"

#include "libtcod.hpp"
#include "engine.hpp"
#include "main.hpp"

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

Engine engine;

int main() {
	sf::Window window(sf::VideoMode(800, 600), "My window");
	while(!TCODConsole::isWindowClosed()) {
		engine.update();
	}

	return 0;
}
