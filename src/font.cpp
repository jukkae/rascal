#include "font.hpp"
#include <iostream>

sf::Font font::mainFont;

extern void font::initialize() {
	std::cout << "font::initialize\n";
	if(!font::mainFont.loadFromFile("assets/FSEX300.ttf")) {
		std::cout << "error loading font\n";
	} else std::cout << "font loaded!\n";
}
