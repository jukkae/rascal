#include "level_up_menu_state.hpp"

#include <iostream>
#include "engine.hpp"
#include "actor.hpp"
#include "ai.hpp"

void LevelUpMenuState::init(Engine* engine) {
	menuContents.push_back({MenuItemType::CONSTITUTION, "constitution"});
	menuContents.push_back({MenuItemType::STRENGTH, "strength"});
	menuContents.push_back({MenuItemType::AGILITY, "agility"});
	menuContents.push_back({MenuItemType::SPEED, "speed"});
	selectedItem = 0;
}


void LevelUpMenuState::cleanup() {

}

void LevelUpMenuState::handleEvents(Engine* engine) {
	TCOD_key_t key;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr);

	switch(key.vk) {
		case TCODK_UP:
			if(selectedItem > 0) selectedItem--;
			break;
		case TCODK_DOWN:
			if(selectedItem < menuContents.size() - 1) selectedItem++;
			break;
		case TCODK_ENTER:
			handleItem(menuContents.at(selectedItem));
			engine->addEngineCommand(ContinueCommand(engine));
			break;
		case TCODK_ESCAPE:
			//engine->addEngineCommand(new ContinueCommand(engine));
			break;
		default:
			break;
	}
}

void LevelUpMenuState::update(Engine* engine) {
	handleEvents(engine);
	render(engine);
}

void LevelUpMenuState::render(Engine* engine) {
	console.setDefaultForeground(TCODColor(200,180,50));
	console.printFrame(0, 0, constants::INVENTORY_WIDTH, constants::INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "level up!");

	int y = 1;
	int itemIndex = 0;
	for (auto item : menuContents) {
		if(itemIndex == selectedItem) console.setDefaultForeground(TCODColor::lighterOrange);
		else console.setDefaultForeground(TCODColor::white);
		console.print(2, y, "%s", item.label.c_str());
		++y;
		++itemIndex;
	}

	TCODConsole::blit(&console, 0, 0, constants::INVENTORY_WIDTH, constants::INVENTORY_HEIGHT, TCODConsole::root, constants::SCREEN_WIDTH/2 - constants::INVENTORY_WIDTH/2, constants::SCREEN_HEIGHT/2 - constants::INVENTORY_HEIGHT/2);
	TCODConsole::flush();
}

void LevelUpMenuState::handleItem(MenuItem item) {
	switch(item.type) {
		case MenuItemType::CONSTITUTION:
			actor->modifyStatistic(Statistic::CONSTITUTION, 20.0f);
			break;
		case MenuItemType::STRENGTH:
			actor->modifyStatistic(Statistic::STRENGTH, 1.0f);
			break;
		case MenuItemType::AGILITY:
			actor->modifyStatistic(Statistic::AGILITY, 1.0f);
			break;
		case MenuItemType::SPEED:
			actor->modifyStatistic(Statistic::SPEED, 10.0f);
			break;
		default: break;
	}
}
