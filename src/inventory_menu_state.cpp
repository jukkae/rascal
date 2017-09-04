#include <iostream>
#include "inventory_menu_state.hpp"
#include "engine.hpp"
#include "actor.hpp"

void InventoryMenuState::init(Engine* engine) {
	for(Actor* item : actor->container->inventory) inventoryContents.push_back(item);
	selectedItem = 0;
}

void InventoryMenuState::cleanup() {

}

void InventoryMenuState::handleEvents(Engine* engine) {
	TCOD_key_t key;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr);

	switch(key.vk) {
		case TCODK_UP:
			if(selectedItem > 0) selectedItem--; // TODO doesn't work and even if it did that'd be bad
			break;
		case TCODK_DOWN:
			if(selectedItem < inventoryContents.size() - 1) selectedItem++;
			break;
		case TCODK_ENTER:
			break;
		case TCODK_ESCAPE:
			engine->addEngineCommand(new ContinueCommand(engine));
			break;
		case TCODK_CHAR: // jesus f. christ, the h*ck we need to deal with
			switch(key.c) {
				case 'd':
					inventoryContents.at(selectedItem)->pickable->drop(inventoryContents.at(selectedItem), actor); // TODO will break with empty inventory
					break;
				case 'e':
					// EQUIP
					break;
				case 't':
					// THROW
					break;
				case 'u':
					// implementation is simple: add useItem command to actor
					inventoryContents.at(selectedItem)->pickable->use(inventoryContents.at(selectedItem), actor); // TODO this does not work *properly* with states
					break;
			}
			break;
		default:
			break;
	}
}

void InventoryMenuState::update(Engine* engine) {

}

void InventoryMenuState::render(Engine* engine) {
	console.setDefaultForeground(TCODColor(200,180,50));
	console.printFrame(0, 0, constants::INVENTORY_WIDTH, constants::INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "inventory");

	int y = 1;
	int itemIndex = 0;
	for (auto item : inventoryContents) {
		if(itemIndex == selectedItem) console.setDefaultForeground(TCODColor::lighterOrange);
		else console.setDefaultForeground(TCODColor::white);
		console.print(2, y, "%s", item->name.c_str());
		++y;
		++itemIndex;
	}

	TCODConsole::blit(&console, 0, 0, constants::INVENTORY_WIDTH, constants::INVENTORY_HEIGHT, TCODConsole::root, constants::SCREEN_WIDTH/2 - constants::INVENTORY_WIDTH/2, constants::SCREEN_HEIGHT/2 - constants::INVENTORY_HEIGHT/2);
	TCODConsole::flush();
}
