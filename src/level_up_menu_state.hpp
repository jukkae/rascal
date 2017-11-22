#ifndef LEVEL_UP_MENU_STATE
#define LEVEL_UP_MENU_STATE
#include "constants.hpp"
#include "state.hpp"
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

class Engine;
class Actor;

enum class MenuItemType { NONE, CONSTITUTION, STRENGTH, AGILITY, SPEED };
struct MenuItem {
	MenuItemType type;
	std::string label;
};

class LevelUpMenuState : public State {
public:
	LevelUpMenuState(Actor* actor);
	void init(Engine* engine) override;

	void handleEvents(Engine* engine) override;
	void update(Engine* engine, sf::RenderWindow& window) override;
	void render(Engine* engine, sf::RenderWindow& window) override;
private:
	void handleItem(MenuItem item);

	Actor* actor;

	std::vector<MenuItem> menuContents;
	int selectedItem;
};
#endif /* LEVEL_UP_MENU_STATE */
