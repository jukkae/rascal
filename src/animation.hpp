#ifndef ANIMATION_HPP
#define ANIMATION_HPP
#include <vector>
#include "colors.hpp"

struct Animation {
	std::vector<char> chars;
	std::vector<sf::Color> colors;
	int charFreq;
	int colFreq;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & chars;
		ar & colors;
		ar & charFreq;
		ar & colFreq;
	}
};

#endif /* ANIMATION_HPP */
