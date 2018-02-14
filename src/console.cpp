#include "console.hpp"

#include "engine.hpp" // TODO need to include this before io.hpp
#include "io.hpp"

#include <sstream>

Console::Console(ConsoleType consoleType, ClearMode clearMode):
consoleType(consoleType), clearMode(clearMode) {
	height = constants::SCREEN_HEIGHT;
	if(consoleType == ConsoleType::NARROW) width = constants::SCREEN_WIDTH;
	else width = constants::SQUARE_SCREEN_WIDTH;
	cells.w = width;
	cells.h = height;
	cells.contents = std::vector<Cell>(width * height);
}

void Console::clear() {
	int cw = consoleType == ConsoleType::NARROW ? constants::CELL_WIDTH : constants::SQUARE_CELL_WIDTH;
	int ch = consoleType == ConsoleType::NARROW ? constants::CELL_HEIGHT : constants::SQUARE_CELL_HEIGHT;

	sf::RectangleShape bg(sf::Vector2f(cw * width, ch * height));
	bg.setPosition(x * cw, y * ch);
	if(clearMode == ClearMode::BLACK) {
		bg.setFillColor(colors::black);
	}
	else {

	}
	io::window.draw(bg);
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
				cells.at(x, y) = cell;
			}
			++x;
		}
		++y;
	}
}

void Console::highlight(Point position, sf::Color color) {

}
