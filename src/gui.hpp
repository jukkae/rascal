#ifndef GUI_HPP
#define GUI_HPP

#include "console.hpp"
#include "messaging.hpp"
#include <SFML/Graphics.hpp>
class GameplayState;
class World;
struct Event;

class Gui {
public:
	Gui();

	void clear();

	void render(World* world, sf::RenderWindow* window);
	void message(sf::Color col, std::string text, ...);
	void message(sf::Color col, std::string text, va_list args);
	void message(Message m); // TODO reference instead of value?

	void setState(GameplayState* s) { state = s; }

	void notify(Event& event);
protected:

	std::vector<Message> log;

	void renderBar(int x, int y, int width, std::string name, float value, float maxValue, const sf::Color barColor, const sf::Color backColor, sf::RenderWindow* window);
	void renderMessageLog(sf::RenderWindow* window);
	void renderXpBar(World* world, sf::RenderWindow* window);
	void renderMouseLook(World* world, sf::RenderWindow* window);
	void renderStats(World* world, sf::RenderWindow* window);
	void renderStatusEffects(World* world, sf::RenderWindow* window);
	void renderNav(World* world, sf::RenderWindow* window);

private:
	bool isInteresting(Event& event);
	GameplayState* state;
	Console console;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & log;
	}
};
#endif /* GUI_HPP */
