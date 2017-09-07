#ifndef GUI_HPP
#define GUI_HPP

#include "actor.hpp" // must be included for serialization
#include "constants.hpp"
class GameplayState;

class Menu {
public:
	enum class MenuItemCode { NONE, CONSTITUTION, STRENGTH, AGILITY, SPEED };
	enum class DisplayMode { PAUSE };
	~Menu();
	void clear();
	void addItem(MenuItemCode code, const std::string label);
	MenuItemCode pick(DisplayMode mode = DisplayMode::PAUSE);

protected:
	struct MenuItem {
		MenuItemCode code;
		std::string label;
	};
	std::vector<MenuItem> items;
};

class Gui {
public:
	Gui(): con(constants::SCREEN_WIDTH, constants::GUI_PANEL_HEIGHT) {;}

	~Gui();
	void clear();

	void render(); // TODO this should be const
	void message(const TCODColor& col, std::string text, ...);
	void message(const TCODColor& col, std::string text, va_list args);

	Menu menu;
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

	TCODConsole con;
	std::vector<Message> log;

	void renderBar(int x, int y, int width, std::string name, float value, float maxValue, const TCODColor& barColor, const TCODColor& backColor);
	void renderMessageLog();
	void renderXpBar();
	void renderMouseLook(std::vector<Actor*>* actors);

private:
	GameplayState* state;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & log;
	}
};
#endif /* GUI_HPP */
