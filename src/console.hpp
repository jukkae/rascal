#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "constants.hpp"
#include "colors.hpp"
#include "mat.hpp"
#include "point.hpp"

struct Cell {
	sf::Color bg = colors::get("black");
	sf::Color fg = colors::get("white");
	char glyph = ' '; // is char good?
};

enum class ConsoleType { SQUARE, NARROW };
enum class ClearMode { TRANSPARENT, BLACK }; // TODO transparency alpha, other colors
class Console {
public:
	Console(ConsoleType consoleType = ConsoleType::NARROW, ClearMode clearMode = ClearMode::BLACK);
	void clear();
	void draw();
	void setBackground(Point position, sf::Color color);
	void drawGlyph(Point position, char glyph, sf::Color color);
	void drawText(Point position, std::string text, sf::Color color);
	void drawGraphicsBlock(Point position, std::string text, sf::Color color);
	void highlight(Point position, sf::Color color = colors::get("darkestGrey"));
	int x = 0; //private, ctor
	int y = 0;
	int width;
	int height;
private:
	//internal 2d buffer of bg/fg colors & glyphs to be drawn
	//-> custom 2d array type?
	Mat2d<Cell> cells;
	ConsoleType consoleType;
	ClearMode clearMode;
};

#endif /* CONSOLE_HPP */
