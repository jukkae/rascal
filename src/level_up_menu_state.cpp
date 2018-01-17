#include "level_up_menu_state.hpp"

#include <iostream>
#include "body.hpp"
#include "destructible.hpp"
#include "engine.hpp"
#include "engine_command.hpp"
#include "actor.hpp"
#include "ai.hpp"
#include "attribute.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "dice.hpp"
#include "effect.hpp"
#include "font.hpp"
#include "status_effect.hpp"
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

LevelUpMenuState::LevelUpMenuState(Engine* engine, Actor* actor) : 
State(engine, engine->getWindow()),
actor(actor) {
	menuContents.push_back({Attribute::STRENGTH, "strength"});
	menuContents.push_back({Attribute::PERCEPTION, "perception"});
	menuContents.push_back({Attribute::ENDURANCE, "endurance"});
	menuContents.push_back({Attribute::CHARISMA, "charisma"});
	menuContents.push_back({Attribute::INTELLIGENCE, "intelligence"});
	menuContents.push_back({Attribute::AGILITY, "agility"});
	menuContents.push_back({Attribute::LUCK, "luck"});
	menuContents.push_back({Attribute::SPEED, "speed"});
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
	actor->modifyAttribute(item.attribute, 1);
	if(auto a = dynamic_cast<PlayerAi*>(actor->ai.get())) {
		int newLevel = a->xpLevel;
		int prevHp = actor->destructible->maxHp;
		int dice = 3 + newLevel + actor->body->getModifier(actor->body->endurance);
		int newHp = 0;
		for(int i = 0; i < dice; ++i) newHp += d4();
		if(newHp > prevHp) actor->destructible->maxHp = newHp;
	}
}
