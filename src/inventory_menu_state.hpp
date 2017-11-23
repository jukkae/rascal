#ifndef INVENTORY_MENU_STATE_HPP
#define INVENTORY_MENU_STATE_HPP
#include "state.hpp"
#include "constants.hpp"
#include <vector>
#include <SFML/Graphics.hpp>

class Engine;
class Actor;
class InventoryMenuState : public State {
public:
	InventoryMenuState(Engine* engine, Actor* actor);

	void handleEvents() override;
	void update() override;
	void render() override;
private:
	Actor* actor;

	std::vector<Actor*> inventoryContents;
	int selectedItem;
};
#endif /* INVENTORY_MENU_STATE_HPP */
