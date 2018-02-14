#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "constants.hpp"
#include "colors.hpp"
#include "point.hpp"
enum class ConsoleType { SQUARE, NARROW };

struct Cell {
	sf::Color bg = colors::black;
	sf::Color fg = colors::white;
	char glyph = ' '; // is char good?
};

template<class T>
class Mat2d {
public:
	using data_type       = std::vector<T>;
	using value_type      = typename std::vector<T>::value_type;
	using size_type       = typename std::vector<T>::size_type;
	using reference       = typename std::vector<T>::reference;
	using const_reference = typename std::vector<T>::const_reference;

	reference operator() (size_type const x, size_type const y) {
		return contents[x + y*w];
	}
	const_reference operator() (size_type const x, size_type const y) const {
		return contents[x + y*x];
	}
	reference at(size_type const x, size_type const y) {
		return contents.at(x + y*w);
	}
	const_reference at(size_type const x, size_type const y) const {
		return contents.at(x + y*w);
	}

	int w;
	int h;
	std::vector<T> contents; //sb private
};

class Console {
public:
	Console(ConsoleType consoleType = ConsoleType::NARROW);
	void draw();
	void setBackground(Point position, sf::Color color);
	void drawGlyph(Point position, char glyph, sf::Color color);
	void drawText(Point position, std::string text, sf::Color color);
	void drawGraphicsBlock(Point position, std::string text, sf::Color color);
	void highlight(Point position, sf::Color color /* = some default */);
private:
	//internal 2d buffer of bg/fg colors & glyphs to be drawn
	//-> custom 2d array type?
	Mat2d<Cell> cells;
	ConsoleType consoleType;
};

#endif /* CONSOLE_HPP */
