#ifndef INVENTORY_MENU_STATE_HPP
#define INVENTORY_MENU_STATE_HPP
#include "state.hpp"
#include "constants.hpp"
#include <vector>
#include "libtcod.hpp"

class Engine;
class InventoryMenuState : public State {
public:
	InventoryMenuState() : console(constants::INVENTORY_WIDTH, constants::INVENTORY_HEIGHT) {;}
	void init() override;
	void cleanup() override;

	void handleEvents(Engine* engine) override;
	void update(Engine* engine) override;
	void render(Engine* engine) override;
private:
	TCODConsole	console;
};
#endif /* INVENTORY_MENU_STATE_HPP */
