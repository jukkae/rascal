#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "colors.hpp"
#include "point.hpp"
class Console {
public:
	void draw();
	void setBackground(Point position, sf::Color color);
	void drawGlyph(Point position, std::string glyph, sf::Color color);
private:
	//internal 2d buffer of bg/fg colors & glyphs to be drawn
	//-> custom 2d array type?
};

#endif /* CONSOLE_HPP */
