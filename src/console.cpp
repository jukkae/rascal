#include "console.hpp"

#include "engine.hpp" // TODO need to include this before io.hpp
#include "io.hpp"

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
	int index = position.x + position.y * constants::SCREEN_WIDTH;
	for(char& c : text) {
		if(c == '\n') {
			index += constants::SCREEN_WIDTH - index % constants::SCREEN_WIDTH;
			index += position.x;
			continue;
		}
		Cell cell;
		cell.fg = color;
		cell.glyph = c;
		cells.contents.at(index) = cell;
		++index;
	}
	//io::text(text, position.x, position.y, color);
}

void Console::highlight(Point position, sf::Color color) {

}
