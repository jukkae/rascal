#ifndef LEVEL_UP_MENU_STATE
#define LEVEL_UP_MENU_STATE
#include "state.hpp"
#include "attribute.hpp"




class Engine;
class Actor;

struct MenuItem {
	Attribute attribute;
	std::string label;
};

class LevelUpMenuState : public State {
public:
	LevelUpMenuState(Engine* engine, Actor* actor);

	void handleEvents() override;
	void update() override;
	void render() override;
private:
	void handleItem(MenuItem item);

	Actor* actor;

	std::vector<MenuItem> menuContents;
	int selectedItem;
};
#endif /* LEVEL_UP_MENU_STATE */
