#include "actor.hpp"
#include "constants.hpp"
#include "effect.hpp"
#include "font.hpp"
#include "gameplay_state.hpp"
#include "io.hpp"
#include "point.hpp"
#include "status_effect.hpp"
#include <SFML/Graphics.hpp>
#include <sys/stat.h>
#include <cstdio>

bool io::waitForMouseClick(GameplayState* state) {
	while(true) {
		sf::Event event;
		while(window.pollEvent(event)) { // Dummy polling to keep macOS happy
			state->render();
			if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				return true;
			}
			if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				return false;
			}
		}
	}
}

bool io::fileExists(std::string name) {
	struct stat buf;
	int result = stat (name.c_str(), &buf);

	if(result == 0) return true;
	else return false;
}

bool io::removeFile(std::string name) {
	int result = remove(name.c_str());
	if(result == 0) return true;
	else return false;
}

void io::text(std::string text, int x, int y, sf::Color color) {
	int verticalOffset = 3;
	sf::Text sfText(text, font::mainFont, 16);
	sfText.setPosition(x*constants::CELL_WIDTH, y*constants::CELL_HEIGHT - verticalOffset);
	sfText.setFillColor(color);
	window.draw(sfText);
}

void io::squareText(std::string text, int x, int y, sf::Color color) {
	int verticalOffset = 3;
	sf::Text sfText(text, font::squareFont, 16);
	sfText.setPosition(x*constants::CELL_WIDTH, y*constants::CELL_HEIGHT - verticalOffset);
	sfText.setFillColor(color);
	window.draw(sfText);
}
