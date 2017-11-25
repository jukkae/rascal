#include <fstream>
#include <sstream>
#include <iostream>

#include "engine.hpp"
#include "main.hpp"
#include "constants.hpp"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

int width = constants::CELL_WIDTH * constants::SCREEN_WIDTH;
int height = constants::CELL_HEIGHT * constants::SCREEN_HEIGHT;
sf::RenderWindow window(sf::VideoMode(width, height), "Rascal");
Engine engine(&window);

int main() {
	window.setMouseCursorVisible(false);
	while(window.isOpen()) {
		sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
			else engine.addEvent(event);
        }
		engine.update();
	}

	return 0;
}
