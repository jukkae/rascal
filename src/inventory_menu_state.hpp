#ifndef INVENTORY_MENU_STATE_HPP
#define INVENTORY_MENU_STATE_HPP
#include "state.hpp"
#include "constants.hpp"
#include <vector>
#include "libtcod.hpp"

class Engine;
class Actor;
class InventoryMenuState : public State {
public:
	InventoryMenuState(Actor* actor) : console(constants::INVENTORY_WIDTH, constants::INVENTORY_HEIGHT), actor(actor) {;}
	void init() override;
	void cleanup() override;

	void handleEvents(Engine* engine) override;
	void update(Engine* engine) override;
	void render(Engine* engine) override;
private:
	TCODConsole	console;
	Actor* actor;
};
#endif /* INVENTORY_MENU_STATE_HPP */
