#ifndef MESSAGING_HPP
#define MESSAGING_HPP
#include <string> // bad

struct Message {
	std::string text;
	sf::Color col;

	Message(std::string text = "", sf::Color col = sf::Color::White):
		text(text), col(col) {;}

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & text;
		ar & col;
	}
};

struct Event;
namespace messaging {
	std::string formatString(std::string text, ...);

	Message createMessageFromEvent(Event& event);
} // namespace message
#endif /* MESSAGING_HPP */
