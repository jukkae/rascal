#include <fstream>
#include <sstream>
#include <iostream>

#include "engine.hpp"
#include "constants.hpp"
#include "io.hpp"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


int width = constants::CELL_WIDTH * constants::SCREEN_WIDTH;
int height = constants::CELL_HEIGHT * constants::SCREEN_HEIGHT;
namespace io { sf::RenderWindow window(sf::VideoMode(width, height), "Rascal"); }
namespace io { Point mousePosition(0,0); }
namespace io { Point mousePositionInScreenCoords(0,0); }
namespace io { Engine* engine; }

int main() {
	{ // Need to have engine go out of scope before exit
		Engine engine(&io::window);
		io::engine = &engine;
		io::window.setMouseCursorVisible(false);
		while(io::window.isOpen()) {
			sf::Event event;
			while (io::window.pollEvent(event)) {
				if (event.type == sf::Event::Closed)
					io::window.close();
				else engine.addEvent(event);
			}
			engine.update();
		}
	}

	return 0;
}
