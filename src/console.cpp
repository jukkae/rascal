#include "console.hpp"

#include "engine.hpp" // TODO need to include this before io.hpp
#include "io.hpp"

void Console::draw() {

}

void Console::setBackground(Point position, sf::Color color) {

}

void Console::drawGlyph(Point position, char glyph, sf::Color color) {

}

void Console::drawText(Point position, std::string text, sf::Color color) {
	io::text(text, position.x, position.y, color);
}

void Console::highlight(Point position, sf::Color color) {

}
