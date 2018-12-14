#ifndef COLORS_HPP
#define COLORS_HPP
#include <SFML/Graphics.hpp>

// Move to two-tiered coloring scheme at some point

namespace colors {
	using sf::Color;

	static Color white            (255, 255, 255);
	static Color lightestGrey     (223, 223, 223);
	static Color lighterGrey      (191, 191, 191);
	static Color lightGrey        (159, 159, 159);
	static Color grey             (127, 127, 127);
	static Color darkGrey         ( 95,  95,  95);
	static Color darkerGrey       ( 63,  63,  63);
	static Color darkestGrey      ( 31,  31,  31);
	static Color black            (  0,   0,   0);

	static Color lightestRed      (255, 191, 191);
	static Color lighterRed       (255, 166, 166);
	static Color lightRed         (255, 115, 115);
	static Color red              (255,   0,   0);
	static Color darkRed          (191,   0,   0);
	static Color darkerRed        (128,   0,   0);
	static Color darkestRed       ( 64,   0,   0);
	static Color desaturatedRed   (128,  64, 128);

	static Color lightestGreen    (191, 255, 191);
	static Color lighterGreen     (166, 255, 166);
	static Color lightGreen       (115, 255, 115);
	static Color green            (  0, 255,   0);
	static Color darkGreen        (  0, 191,   0);
	static Color darkerGreen      (  0, 128,   0);
	static Color darkestGreen     (  0,  64,   0);
	static Color desaturatedGreen ( 64, 128,  64);

	static Color lightestBlue     (191, 191, 255);
	static Color lighterBlue      (166, 166, 255);
	static Color lightBlue        (115, 115, 255);
	static Color blue             (  0,   0, 255);
	static Color darkBlue         (  0,   0, 191);
	static Color darkerBlue       (  0,   0, 128);
	static Color darkestBlue      (  0,   0,  64);
	static Color desaturatedBlue  ( 64,  64, 128);

	static Color brightBlue       (100, 100, 255);
	static Color cyan             (  0, 255, 255);
	static Color yellow           (255, 255,   0);
	static Color darkerYellow     (128, 128,   0);

	static sf::Color darkWall     (0,   0,   100);
	static sf::Color darkGround   (100, 110, 100);
	static sf::Color lightWall    (100, 100, 200);
	static sf::Color lightGround  (200, 210, 200);

	static const inline Color multiply(Color c, float f) { return Color(c.r * f  , c.g * f  , c.b * f  ); }
	static const inline Color      add(Color c, Color d) { return Color(c.r + d.r, c.g + d.g, c.b + d.b); }
};

#endif /* COLORS_HPP */
