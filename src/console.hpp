#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "colors.hpp"
#include "point.hpp"
enum class ConsoleType { SQUARE, NARROW };

struct Cell {
	sf::Color bg;
	sf::Color fg;
	char glyph; // is char good?
};

class Console {
public:
	Console() : consoleType(ConsoleType::SQUARE) {;}
	void draw();
	void setBackground(Point position, sf::Color color);
	void drawGlyph(Point position, std::string glyph, sf::Color color);
	void highlight(Point position, sf::Color color /* = some default */);
private:
	//internal 2d buffer of bg/fg colors & glyphs to be drawn
	//-> custom 2d array type?
	std::vector<Cell> cells;
	ConsoleType consoleType;
};

#endif /* CONSOLE_HPP */
