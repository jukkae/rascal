#ifndef MESSAGING_HPP
#define MESSAGING_HPP

struct Message {
	std::string text;
	sf::Color col;

	Message(std::string text = "", sf::Color col = sf::Color::White);

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & text;
		ar & col;
	}
};

struct Event;
namespace messaging {
	Message createMessageFromEvent(Event& event);
} // namespace message
#endif /* MESSAGING_HPP */
