#ifndef COLORS_HPP
#define COLORS_HPP
#include <SFML/Graphics.hpp>

// Move to two-tiered coloring scheme at some point

namespace colors {
	using sf::Color;

	static const std::map<std::string, Color> namedColors = {
		{ "white",            Color(255, 255, 255) },
		{ "lightestGrey",     Color(223, 223, 223) },
		{ "lighterGrey",      Color(191, 191, 191) },
		{ "lightGrey",        Color(159, 159, 159) },
		{ "grey",             Color(127, 127, 127) },
		{ "darkGrey",         Color( 95,  95,  95) },
		{ "darkerGrey",       Color( 63,  63,  63) },
		{ "darkestGrey",      Color( 31,  31,  31) },
		{ "black",            Color(  0,   0,   0) },

		{ "lightestRed",      Color(255, 191, 191) },
		{ "lighterRed",       Color(255, 166, 166) },
		{ "lightRed",         Color(255, 115, 115) },
		{ "red",              Color(255,   0,   0) },
		{ "darkRed",          Color(191,   0,   0) },
		{ "darkerRed",        Color(128,   0,   0) },
		{ "darkestRed",       Color( 64,   0,   0) },
		{ "desaturatedRed",   Color(128,  64,  64) },

		{ "lightestGreen",    Color(191, 255, 191) },
		{ "lighterGreen",     Color(166, 255, 166) },
		{ "lightGreen",       Color(115, 255, 115) },
		{ "green",            Color(  0, 255,   0) },
		{ "darkGreen",        Color(  0, 191,   0) },
		{ "darkerGreen",      Color(  0, 128,   0) },
		{ "darkestGreen",     Color(  0,  64,   0) },
		{ "desaturatedGreen", Color( 64, 128,  64) },

		{ "lightestBlue",     Color(191, 191, 255) },
		{ "lighterBlue",      Color(166, 166, 255) },
		{ "lightBlue",        Color(115, 115, 255) },
		{ "blue",             Color(  0,   0, 255) },
		{ "darkBlue",         Color(  0,   0, 191) },
		{ "darkerBlue",       Color(  0,   0, 128) },
		{ "darkestBlue",      Color(  0,   0,  64) },
		{ "desaturatedBlue",  Color( 64,  64, 128) },
		{ "brightBlue",       Color(100, 100, 255) },

		{ "cyan",             Color(  0, 255, 255) },
		{ "darkerCyan",       Color(  0, 128, 128) },
		{ "yellow",           Color(255, 255,   0) },
		{ "darkerYellow",     Color(128, 128,   0) },

		{ "darkWall",         Color(  0,   0, 100) },
		{ "darkGround",       Color(100, 110, 100) },
		{ "lightWall",        Color(100, 100, 200) },
		{ "lightGround",      Color(200, 210, 200) },
	};

	static const inline Color get(std::string name) { return namedColors.at(name); }

	static const inline Color multiply(Color c, float f) { return Color(c.r * f  , c.g * f  , c.b * f  ); }
	static const inline Color      add(Color c, Color d) { return Color(c.r + d.r, c.g + d.g, c.b + d.b); }
};

#endif /* COLORS_HPP */
