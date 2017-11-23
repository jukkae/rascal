#include <iostream>
#include "inventory_menu_state.hpp"
#include "engine.hpp"
#include "action.hpp"
#include "actor.hpp"
#include "colors.hpp"
#include "font.hpp"

InventoryMenuState::InventoryMenuState(Engine* engine, Actor* actor) :
State(engine, engine->getWindow()),
actor(actor) {
	inventoryContents.clear();
	for(auto& item : actor->container->inventory) inventoryContents.push_back(item.get());
	selectedItem = 0;
}

void InventoryMenuState::handleEvents() {
	sf::Event event;
	while(engine->pollEvent(event)) {
		if(event.type == sf::Event::KeyPressed) {
			using k = sf::Keyboard::Key;
			switch(event.key.code) {
				case k::Up:
					if(selectedItem > 0) --selectedItem;
					break;
				case k::Down:
					if(selectedItem < inventoryContents.size() - 1) ++selectedItem;
					break;
				case k::D:
					if(inventoryContents.size() > 0) {
						std::unique_ptr<Actor> item = std::move(actor->container->inventory.at(selectedItem));
						actor->container->inventory.erase(actor->container->inventory.begin() + selectedItem); // TODO ugh
						inventoryContents.at(selectedItem)->pickable->drop(std::move(item), actor);
						//inventoryContents.at(selectedItem)->pickable->drop(inventoryContents.at((selectedItem)), actor);
						engine->addEngineCommand(ContinueCommand(engine));
					}
					break;
				case k::U:
					if(inventoryContents.size() > 0) {
						actor->addAction(std::make_unique<UseItemAction>(UseItemAction(actor, inventoryContents.at(selectedItem))));
						engine->addEngineCommand(ContinueCommand(engine));
					}
					break;
				case k::Escape:
					engine->addEngineCommand(ContinueCommand(engine));
					break;
				default:
					break;
			}
		}
	}
}

void InventoryMenuState::update() {
	handleEvents();
	render();
}

void InventoryMenuState::render() {
	window->clear(sf::Color::Black);

	sf::Text heading("I N V E N T O R Y", font::mainFont, 16);
	heading.setPosition(2*constants::CELL_WIDTH, constants::CELL_HEIGHT);
	heading.setFillColor(colors::brightBlue);
	window->draw(heading);

	int y = 1;
	int itemIndex = 0;
	for (auto item : inventoryContents) {
		sf::Text text(item->name, font::mainFont, 16);
		text.setPosition(2*constants::CELL_WIDTH, (y+2)*constants::CELL_HEIGHT);
		if(itemIndex == selectedItem) text.setFillColor(colors::brightBlue);
		else text.setFillColor(colors::darkBlue);
		window->draw(text);
		++y;
		++itemIndex;
	}

	window->display();
}
