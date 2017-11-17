#ifndef COLORS_HPP
#define COLORS_HPP
#include <SFML/Graphics.hpp>

// Move to two-tiered coloring scheme at some point

namespace colors {
	using sf::Color;
	static Color lightGrey        (127, 127, 127);
	static Color cyan             (  0, 255, 255);
	static Color red              (255,   0,   0);
	static Color white            (255, 255, 255);
	static Color yellow           (255, 255,   0);
	static Color green            (  0, 255,   0);
	static Color brightBlue       (100, 100, 255);
	static Color darkBlue         (  0,   0, 155);
	static Color desaturatedGreen ( 64, 128,  64);
	static Color darkGreen        (  0, 128,   0);
	static Color darkerGreen      (  0,  64,   0);
};

#endif /* COLORS_HPP */
