#include <iostream>
#include "inventory_menu_state.hpp"
#include "engine.hpp"
#include "action.hpp"
#include "actor.hpp"

static const sf::Color brightBlue(100, 100, 255);
static const sf::Color darkBlue(0, 0, 155);

InventoryMenuState::InventoryMenuState(Actor* actor) : actor(actor) {
	if(!font.loadFromFile("assets/FSEX300.ttf")) {
		std::cout << "error loading font\n";
	} else std::cout << "font loaded!\n";
}

void InventoryMenuState::init(Engine* engine) {
	inventoryContents.clear();
	for(Actor* item : actor->container->inventory) inventoryContents.push_back(item);
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
						inventoryContents.at(selectedItem)->pickable->drop(inventoryContents.at(selectedItem), actor);
						engine->addEngineCommand(ContinueCommand(engine));
					}
					break;
				case k::U:
					if(inventoryContents.size() > 0) {
						actor->addAction(new UseItemAction(actor, inventoryContents.at(selectedItem)));
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
	//console.setDefaultForeground(TCODColor(200,180,50));
	//console.printFrame(0, 0, constants::INVENTORY_WIDTH, constants::INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "inventory");

	int y = 1;
	int itemIndex = 0;
	for (auto item : inventoryContents) {
		sf::Text text(item->name, font, 16);
		text.setPosition(2*constants::CELL_WIDTH, y*constants::CELL_HEIGHT);
		if(itemIndex == selectedItem) text.setColor(brightBlue);
		else text.setColor(darkBlue);
		window.draw(text);
		++y;
		++itemIndex;
	}

	//TCODConsole::blit(&console, 0, 0, constants::INVENTORY_WIDTH, constants::INVENTORY_HEIGHT, TCODConsole::root, constants::SCREEN_WIDTH/2 - constants::INVENTORY_WIDTH/2, constants::SCREEN_HEIGHT/2 - constants::INVENTORY_HEIGHT/2);
	//TCODConsole::flush();
	window.display();
}
