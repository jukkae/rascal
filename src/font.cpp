#include "font.hpp"
#include <iostream>

sf::Font font::mainFont;

extern void font::load() {
	if(!font::mainFont.loadFromFile("assets/FSEX300.ttf")) {
		std::cout << "error loading font\n";
	}
}
