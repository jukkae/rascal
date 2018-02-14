#include "console.hpp"

#include "engine.hpp" // TODO need to include this before io.hpp
#include "io.hpp"

#include <sstream>

Console::Console(ConsoleType consoleType):
consoleType(consoleType) {
	cells.contents = std::vector<Cell>(constants::SCREEN_WIDTH * constants::SCREEN_HEIGHT);
}

void Console::draw() {
	int index = 0;
	for(auto& c : cells.contents) {
		int x = index % constants::SCREEN_WIDTH;
		int y = index / constants::SCREEN_WIDTH;
		io::text(std::string(1, c.glyph), x, y, c.fg);
		++index;
	}
}

void Console::setBackground(Point position, sf::Color color) {

}

void Console::drawGlyph(Point position, char glyph, sf::Color color) {

}

void Console::drawText(Point position, std::string text, sf::Color color) {

}

void Console::drawGraphicsBlock(Point position, std::string text, sf::Color color) {
	int y = position.y;
	std::istringstream s(text);
	std::string line;
	while(std::getline(s, line)) {
		int x = position.x;
		for(char& c : line) {
			Cell cell;
			cell.fg = color;
			cell.glyph = c;
			if(x < constants::SCREEN_WIDTH) { // if outside, just discard
				cells.contents.at(x + y*constants::SCREEN_WIDTH) = cell;
			}
			++x;
		}
		++y;
	}
}

void Console::highlight(Point position, sf::Color color) {

}
