#include <fstream>
#include <sstream>
#include <iostream>

//#include "libtcod.hpp"
#include "engine.hpp"
#include "main.hpp"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

Engine engine;

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "Rascal");
	while(window.isOpen()) {
		sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
			else engine.addEvent(event);
        }
		engine.update(window);
	}

	return 0;
}
