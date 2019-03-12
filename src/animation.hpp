#ifndef ANIMATION_HPP
#define ANIMATION_HPP
#include <vector>
#include <SFML/Graphics.hpp>

struct Animation {
	std::vector<char> chars;
	std::vector<sf::Color> colors;
	int charFreq;
	int colFreq;
	int phase;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & chars;
		ar & colors;
		ar & charFreq;
		ar & colFreq;
		ar & phase;
	}
};

#endif /* ANIMATION_HPP */
