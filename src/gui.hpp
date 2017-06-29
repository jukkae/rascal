#ifndef GUI_HPP
#define GUI_HPP
class Menu {
public:
	enum class MenuItemCode { NONE, NEW_GAME, CONTINUE, EXIT, CONSTITUTION, STRENGTH, AGILITY };
	enum class DisplayMode { MAIN, PAUSE };
	~Menu();
	void clear();
	void addItem(MenuItemCode code, const std::string label);
	MenuItemCode pick(DisplayMode mode = DisplayMode::MAIN);

protected:
	struct MenuItem {
		MenuItemCode code;
		std::string label;
	};
	std::vector<MenuItem> items;
};

class Gui {
public:
	static const int PANEL_HEIGHT = 7;
	static const int ENGINE_SCREEN_WIDTH = 80; // TODO get this dynamically
	
	Gui(): con(ENGINE_SCREEN_WIDTH, PANEL_HEIGHT) {;}

	~Gui();
	void clear();

	void render(); // TODO this should be const
	void message(const TCODColor &col, std::string text, ...);

	Menu menu;

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
	void renderMouseLook();

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & log;
	}
};
#endif /* GUI_HPP */
