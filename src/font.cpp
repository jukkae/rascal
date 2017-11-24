#include "font.hpp"
#include <iostream>

sf::Font font::mainFont;
sf::Font font::squareFont;

extern void font::load() {
	if(!font::mainFont.loadFromFile("assets/FSEX300.ttf")) {
		std::cout << "error loading main font\n";
	}
	if(!font::squareFont.loadFromFile("assets/square.ttf")) {
		std::cout << "error loading square font\n";
	}
}
