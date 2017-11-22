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

	void handleEvents(Engine* engine) override;
	void update(Engine* engine, sf::RenderWindow& window) override;
	void render(Engine* engine, sf::RenderWindow& window) override;
private:
	Actor* actor;

	std::vector<Actor*> inventoryContents;
	int selectedItem;
};
#endif /* INVENTORY_MENU_STATE_HPP */
