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

template<class T>
class Mat2d {
public:
	std::vector<T> contents; //sb private
};

class Console {
public:
	Console(ConsoleType consoleType = ConsoleType::NARROW) : consoleType(consoleType) {;}
	void draw();
	void setBackground(Point position, sf::Color color);
	void drawGlyph(Point position, char glyph, sf::Color color);
	void highlight(Point position, sf::Color color /* = some default */);
private:
	//internal 2d buffer of bg/fg colors & glyphs to be drawn
	//-> custom 2d array type?
	Mat2d<Cell> cells;
	ConsoleType consoleType;
};

#endif /* CONSOLE_HPP */
