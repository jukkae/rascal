#include "level_up_menu_state.hpp"

#include <iostream>
#include "engine.hpp"
#include "engine_command.hpp"
#include "actor.hpp"
#include "ai.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "effect.hpp"
#include "font.hpp"
#include "status_effect.hpp"
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

LevelUpMenuState::LevelUpMenuState(Engine* engine, Actor* actor) : 
State(engine, engine->getWindow()),
actor(actor) {
	menuContents.push_back({MenuItemType::CONSTITUTION, "constitution"});
	menuContents.push_back({MenuItemType::STRENGTH, "strength"});
	menuContents.push_back({MenuItemType::AGILITY, "agility"});
	menuContents.push_back({MenuItemType::SPEED, "speed"});
	selectedItem = 0;
}

void LevelUpMenuState::handleEvents() {
	sf::Event event;
	while(engine->pollEvent(event)) {
		if(event.type == sf::Event::KeyPressed) {
			using k = sf::Keyboard::Key;
			switch(event.key.code) {
				case k::Up:
					if(selectedItem > 0) --selectedItem;
					break;
				case k::Down:
					if(selectedItem < menuContents.size() - 1) ++selectedItem;
					break;
				case k::Return:
					handleItem(menuContents.at(selectedItem));
					engine->addEngineCommand(ContinueCommand(engine));
					break;
				default:
					break;
			}
		}
	}
}

void LevelUpMenuState::update() {
	handleEvents();
	render();
}

void LevelUpMenuState::render() {
	window->clear(sf::Color::Black);

	int y = 1;
	int itemIndex = 0;
	for (auto item : menuContents) {
		sf::Text text;
		text.setCharacterSize(16);
		text.setFont(font::mainFont);
		text.setString(item.label);
		text.setPosition(2*constants::CELL_WIDTH, y*constants::CELL_HEIGHT);
		if(itemIndex == selectedItem) text.setFillColor(colors::brightBlue);
		else text.setFillColor(colors::darkBlue);
		window->draw(text);
		++y;
		++itemIndex;
	}

	window->display();
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
