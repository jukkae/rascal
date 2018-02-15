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
	for(auto& c : cells) c = Cell();
}

void Console::clear() {
	for(auto& c : cells) c = Cell();
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
	int cw = ((consoleType == ConsoleType::NARROW) ? constants::CELL_WIDTH : constants::SQUARE_CELL_WIDTH);
	int ch = consoleType == ConsoleType::NARROW ? constants::CELL_HEIGHT : constants::SQUARE_CELL_HEIGHT;
	for(auto& c : cells.contents) {
		int xc = index % cells.w;
		int yc = index / cells.w;
		sf::RectangleShape bg(sf::Vector2f(cw, ch));
		bg.setPosition((x+xc) * cw, (y+yc) * ch);
		bg.setFillColor(c.bg);
		io::window.draw(bg);
		if(consoleType == ConsoleType::NARROW) io::text(std::string(1, c.glyph), x+xc, y+yc, c.fg);
		else io::squareText(std::string(1, c.glyph), x+xc*2, y+yc, c.fg);
		++index;
	}
	//io::window.display();
}

void Console::setBackground(Point position, sf::Color color) {
	int x = position.x;
	int y = position.y;
	if(x < 0 || x >= width || y < 0 || y >= height) return;
	cells.at(x, y).bg = color;
}

void Console::drawGlyph(Point position, char glyph, sf::Color color) {
	int x = position.x;
	int y = position.y;
	if(x < 0 || x >= width || y < 0 || y >= height) return;
	cells.at(x, y).glyph = glyph;
	cells.at(x, y).fg = color;
}

void Console::drawText(Point position, std::string text, sf::Color color) {
	// TODO this should break lines automatically etc
	int y = position.y;
	std::istringstream s(text);
	std::string line;
	while(std::getline(s, line)) {
		int x = position.x;
		for(char& c : line) {
			if(x < constants::SCREEN_WIDTH) { // if outside, just discard
				cells.at(x, y).fg = color;
				cells.at(x, y).glyph = c;
			}
			++x;
		}
		++y;
	}
}

void Console::drawGraphicsBlock(Point position, std::string text, sf::Color color) {
	int y = position.y;
	std::istringstream s(text);
	std::string line;
	while(std::getline(s, line)) {
		int x = position.x;
		for(char& c : line) {
			if(x < constants::SCREEN_WIDTH) { // if outside, just discard
				cells.at(x, y).fg = color;
				cells.at(x, y).glyph = c;
			}
			++x;
		}
		++y;
	}
}

void Console::highlight(Point position, sf::Color color) {

}
