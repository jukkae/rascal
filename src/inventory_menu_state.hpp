#ifndef INVENTORY_MENU_STATE_HPP
#define INVENTORY_MENU_STATE_HPP
#include "state.hpp"
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
	void sortIntoPiles();
	void renderPiles();
	void renderStats();
	void renderBodyParts();
	void renderMissions();
	Actor* actor;
	int credits;
	int contentsWeight;
	int capacity;

	std::vector<Actor*> inventoryContents;
	std::vector<std::vector<Actor*>> piles;
	int selectedItem;
};
#endif /* INVENTORY_MENU_STATE_HPP */
