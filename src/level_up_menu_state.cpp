#include "level_up_menu_state.hpp"

#include <iostream>
#include "engine.hpp"
#include "actor.hpp"
#include "ai.hpp"
#include "constants.hpp"

static const sf::Color brightBlue(100, 100, 255);
static const sf::Color darkBlue(0, 0, 155);

LevelUpMenuState::LevelUpMenuState(Actor* actor) : actor(actor) {
	if(!font.loadFromFile("assets/FSEX300.ttf")) {
		std::cout << "error loading font\n";
	} else std::cout << "font loaded!\n";
}

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

void LevelUpMenuState::update(Engine* engine, sf::RenderWindow& window) {
	handleEvents(engine);
	render(engine, window);
}

void LevelUpMenuState::render(Engine* engine, sf::RenderWindow& window) {
	window.clear(sf::Color::Black);

	int y = 1;
	int itemIndex = 0;
	for (auto item : menuContents) {
		sf::Text text;
		text.setCharacterSize(16);
		text.setFont(font);
		text.setString(item.label);
		text.setPosition(2*constants::CELL_WIDTH, y*constants::CELL_HEIGHT);
		if(itemIndex == selectedItem) text.setColor(brightBlue);
		else text.setColor(darkBlue);
		window.draw(text);
		++y;
		++itemIndex;
	}

	window.display();
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
