#include <iostream>
#include "inventory_menu_state.hpp"
#include "engine.hpp"

void InventoryMenuState::init() {

}

void InventoryMenuState::cleanup() {

}

void InventoryMenuState::handleEvents(Engine* engine) {

}

void InventoryMenuState::update(Engine* engine) {

}

void InventoryMenuState::render(Engine* engine) {
	console.setDefaultForeground(TCODColor(200,180,50));
	console.printFrame(0, 0, constants::INVENTORY_WIDTH, constants::INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "inventory");

	/*
	console.setDefaultForeground(TCODColor::white);
	int shortcut = 'a';
	int y = 1;
	for (Actor* actor : owner->container->inventory) {
		con.print(2, y, "(%c) %s", shortcut, actor->name.c_str());
		++y;
		++shortcut;
	}
	// blit the inventory console on the root console
	*/
	TCODConsole::blit(&console, 0, 0, constants::INVENTORY_WIDTH, constants::INVENTORY_HEIGHT, TCODConsole::root, constants::SCREEN_WIDTH/2 - constants::INVENTORY_WIDTH/2, constants::SCREEN_HEIGHT/2 - constants::INVENTORY_HEIGHT/2);
	TCODConsole::flush();
	// wait for a key press
	/*
	TCOD_key_t key;

	boost::optional<RawInputEvent> event = owner->s->inputHandler->getEvent();
	key = event->key; // This probably doesn't work, but then again this whole code is not called rn

	if(key.vk == TCODK_CHAR) {
		int actorIndex=key.c - 'a';
		if(actorIndex >= 0 && actorIndex < owner->container->inventory.size()) { return owner->container->inventory.at(actorIndex); }
	}
	*/
}
