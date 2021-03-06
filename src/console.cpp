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
	if(clearMode == ClearMode::BLACK) {
		for(auto& c : cells) c = Cell();
	}
	if(clearMode == ClearMode::TRANSPARENT) {
		for(auto& c : cells) c = Cell{sf::Color(0,0,0,0), colors::get("black"), ' '};
	}
}

void Console::clear() {
	// TODO what should this function really do? Who knows?
	// Doesn't work w/ transparent
	if(clearMode == ClearMode::BLACK) {
		for(auto& c : cells) c = Cell();
	}
	if(clearMode == ClearMode::TRANSPARENT) {
		for(auto& c : cells) c = Cell{sf::Color(0,0,0,0), colors::get("black"), ' '};
	}
	int cw = consoleType == ConsoleType::NARROW ? constants::CELL_WIDTH : constants::SQUARE_CELL_WIDTH;
	int ch = consoleType == ConsoleType::NARROW ? constants::CELL_HEIGHT : constants::SQUARE_CELL_HEIGHT;

	sf::RectangleShape bg(sf::Vector2f(cw * width, ch * height));
	bg.setPosition(x * cw, y * ch);
	if(clearMode == ClearMode::BLACK) {
		bg.setFillColor(colors::get("black"));
	}
	else {
		bg.setFillColor(sf::Color(0,0,0,0));
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

sf::Color Console::getBackground(Point position) {
	int x = position.x;
	int y = position.y;
	if(x < 0 || x >= width || y < 0 || y >= height) return colors::get("black"); // depends on the context, might throw, might return white, might return transparent... but for now good enough
	return cells.at(x, y).bg;
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
	sf::Color newBg = colors::add(cells.at(position.x, position.y).bg, color);
	setBackground(position, newBg);
}
