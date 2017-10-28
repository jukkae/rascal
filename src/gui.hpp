#ifndef GUI_HPP
#define GUI_HPP

#include "actor.hpp" // must be included for serialization
#include "constants.hpp"
#include <SFML/Graphics.hpp>
class GameplayState;

class Gui {
public:
	Gui();

	~Gui();
	void clear();

	void render(sf::RenderWindow& window); // TODO this should be const
	void message(const TCODColor& col, std::string text, ...);
	void message(const TCODColor& col, std::string text, va_list args);

	void setState(GameplayState* s) { state = s; }
protected:
	struct Message {
		std::string text;
		TCODColor col;

		Message(std::string text = "", const TCODColor& col = TCODColor::white);
		~Message();

		template<typename Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & text;
			ar & col;
		}
	};

	std::vector<Message> log;

	void renderBar(int x, int y, int width, std::string name, float value, float maxValue, const sf::Color barColor, const sf::Color backColor, sf::RenderWindow& window);
	void renderMessageLog(sf::RenderWindow& window);
	void renderXpBar(sf::RenderWindow& window);
	void renderMouseLook(std::vector<Actor*>* actors);

private:
	GameplayState* state;
	sf::Font font;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & log;
	}
};
#endif /* GUI_HPP */
