#include <iostream>
#include "inventory_menu_state.hpp"
#include "engine.hpp"
#include "action.hpp"
#include "actor.hpp"

void InventoryMenuState::init(Engine* engine) {
	inventoryContents.clear();
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
			if(selectedItem > 0) selectedItem--;
			break;
		case TCODK_DOWN:
			if(selectedItem < inventoryContents.size() - 1) selectedItem++;
			break;
		case TCODK_ENTER:
			break;
		case TCODK_ESCAPE:
			engine->addEngineCommand(ContinueCommand(engine));
			break;
		case TCODK_CHAR: // jesus f. christ, the h*ck we need to deal with
			switch(key.c) {
				case 'd':
					if(inventoryContents.size() > 0) {
						inventoryContents.at(selectedItem)->pickable->drop(inventoryContents.at(selectedItem), actor);
						engine->addEngineCommand(ContinueCommand(engine));
					}
					break;
				case 'e':
					// EQUIP
					break;
				case 't':
					// THROW
					break;
				case 'u':
					if(inventoryContents.size() > 0) {
						actor->addAction(new UseItemAction(actor, inventoryContents.at(selectedItem)));
						engine->addEngineCommand(ContinueCommand(engine));
					}
					break;
			}
			break;
		default:
			break;
	}
}

void InventoryMenuState::update(Engine* engine, sf::RenderWindow& window) {
	handleEvents(engine);
	render(engine, window);
}

void InventoryMenuState::render(Engine* engine, sf::RenderWindow& window) {
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
