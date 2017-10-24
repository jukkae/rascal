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
	sf::Font font;
	if(!font.loadFromFile("assets/FSEX300.ttf")) {
		std::cout << "error\n";
	}
	sf::Text text;
	text.setFont(font);
	//text.setString(engine.getText());
	text.setString("hello hello");
	text.setCharacterSize(16);
	text.setColor(sf::Color::Red);
	

	sf::RenderWindow window(sf::VideoMode(800, 600), "Rascal");
	while(window.isOpen()) {
		sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
			else engine.addEvent(event);
        }
		window.clear(sf::Color::Black);
		window.draw(text);
		window.display();
		//engine.update();
	}

	return 0;
}
