#include <iostream>
#include "inventory_menu_state.hpp"
#include "engine.hpp"
#include "action.hpp"
#include "actor.hpp"
#include "colors.hpp"
#include "font.hpp"

InventoryMenuState::InventoryMenuState(Actor* actor) : actor(actor) {
}

void InventoryMenuState::init(Engine* engine) {
	inventoryContents.clear();
	for(auto& item : actor->container->inventory) inventoryContents.push_back(item.get());
	selectedItem = 0;
}

void InventoryMenuState::cleanup() {

}

void InventoryMenuState::handleEvents(Engine* engine) {
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
						inventoryContents.at(selectedItem)->pickable->drop(inventoryContents.at(std::move(selectedItem)), actor);
						engine->addEngineCommand(ContinueCommand(engine));
					}
					break;
				case k::U:
					if(inventoryContents.size() > 0) {
						actor->addAction(std::make_unique<UseItemAction>(UseItemAction(actor, inventoryContents.at(selectedItem))));
						engine->addEngineCommand(ContinueCommand(engine));
					}
					break;
				default:
					break;
			}
		}
	}
}

void InventoryMenuState::update(Engine* engine, sf::RenderWindow& window) {
	handleEvents(engine);
	render(engine, window);
}

void InventoryMenuState::render(Engine* engine, sf::RenderWindow& window) {
	window.clear(sf::Color::Black);

	int y = 1;
	int itemIndex = 0;
	for (auto item : inventoryContents) {
		sf::Text text(item->name, font::mainFont, 16);
		text.setPosition(2*constants::CELL_WIDTH, y*constants::CELL_HEIGHT);
		if(itemIndex == selectedItem) text.setFillColor(colors::brightBlue);
		else text.setFillColor(colors::darkBlue);
		window.draw(text);
		++y;
		++itemIndex;
	}

	window.display();
}
