#include <iostream>
#include "inventory_menu_state.hpp"
#include "engine.hpp"
#include "actor.hpp"

void InventoryMenuState::init(Engine* engine) {
	for(Actor* item : actor->container->inventory) inventoryContents.push_back(item);
}

void InventoryMenuState::cleanup() {

}

void InventoryMenuState::handleEvents(Engine* engine) {

}

void InventoryMenuState::update(Engine* engine) {

}

void InventoryMenuState::render(Engine* engine) {
	selectedItem = inventoryContents.front();
	console.setDefaultForeground(TCODColor(200,180,50));
	console.printFrame(0, 0, constants::INVENTORY_WIDTH, constants::INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "inventory");

	int shortcut = 'a';
	int y = 1;
	for (auto item : inventoryContents) {
		if(item == selectedItem) console.setDefaultForeground(TCODColor::lighterOrange);
		else console.setDefaultForeground(TCODColor::white);
		console.print(2, y, "(%c) %s", shortcut, item->name.c_str());
		++y;
		++shortcut;
	}

	TCODConsole::blit(&console, 0, 0, constants::INVENTORY_WIDTH, constants::INVENTORY_HEIGHT, TCODConsole::root, constants::SCREEN_WIDTH/2 - constants::INVENTORY_WIDTH/2, constants::SCREEN_HEIGHT/2 - constants::INVENTORY_HEIGHT/2);
	TCODConsole::flush();
	// wait for a key press
	/*
	TCOD_key_t key;

	boost::optional<RawInputEvent> event = actor->s->inputHandler->getEvent();
	key = event->key; // This probably doesn't work, but then again this whole code is not called rn

	if(key.vk == TCODK_CHAR) {
		int actorIndex=key.c - 'a';
		if(actorIndex >= 0 && actorIndex < actor->container->inventory.size()) { return actor->container->inventory.at(actorIndex); }
	}
	*/
}
