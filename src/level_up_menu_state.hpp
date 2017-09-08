#ifndef LEVEL_UP_MENU_STATE
#define LEVEL_UP_MENU_STATE
#include "constants.hpp"
#include "state.hpp"
#include <string>
#include <vector>
#include "libtcod.hpp"

class Engine;
class Actor;

enum class MenuItemType { NONE, CONSTITUTION, STRENGTH, AGILITY, SPEED };
struct MenuItem {
	MenuItemType type;
	std::string label;
};

class LevelUpMenuState : public State {
public:
	LevelUpMenuState(Actor* actor) : console(constants::INVENTORY_WIDTH, constants::INVENTORY_HEIGHT), actor(actor) {;}
	void init(Engine* engine) override;
	void cleanup() override;

	void handleEvents(Engine* engine) override;
	void update(Engine* engine) override;
	void render(Engine* engine) override;
private:
	void handleItem(MenuItem item);

	TCODConsole	console;
	Actor* actor;

	std::vector<MenuItem> menuContents;
	int selectedItem;
};
#endif /* LEVEL_UP_MENU_STATE */
