#include <iostream>
#include <algorithm>
#include "inventory_menu_state.hpp"
#include "engine.hpp"
#include "action.hpp"
#include "actor.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "font.hpp"

InventoryMenuState::InventoryMenuState(Engine* engine, Actor* actor) :
State(engine, engine->getWindow()),
actor(actor) {
	inventoryContents.clear();
	for(auto& item : actor->container->inventory) inventoryContents.push_back(item.get());

	std::sort(inventoryContents.begin(), inventoryContents.end(), [](const Actor* lhs, const Actor* rhs)
		{
			return lhs->name < rhs->name;
		}
	);

	sortIntoPiles();

	selectedItem = 0;
	credits = actor->container->credits;
	contentsWeight = actor->container->getContentsWeight();
	capacity = actor->container->capacity;
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
					if(selectedItem < piles.size() - 1) ++selectedItem;
					break;
				case k::D:
					if(inventoryContents.size() > 0) {
						actor->addAction(std::make_unique<DropItemAction>(DropItemAction(actor, piles.at(selectedItem).at(0))));
						engine->addEngineCommand(ContinueCommand(engine));
					}
					break;
				case k::U:
					if(inventoryContents.size() > 0) {
						actor->addAction(std::make_unique<UseItemAction>(UseItemAction(actor, piles.at(selectedItem).at(0))));
						engine->addEngineCommand(ContinueCommand(engine));
					}
					break;
				case k::W:
					if(inventoryContents.size() > 0) {
						actor->addAction(std::make_unique<WieldItemAction>(WieldItemAction(actor, piles.at(selectedItem).at(0))));
						engine->addEngineCommand(ContinueCommand(engine));
					}
					break;
				case k::E:
					if(inventoryContents.size() > 0) {
						actor->addAction(std::make_unique<UnWieldItemAction>(UnWieldItemAction(actor)));
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

	int x = 2;
	int y = 3;

	int itemIndex = 0;
	for (auto pile : piles) {
		sf::Text text(pile.at(0)->name + " (" + std::to_string(pile.size()) + ")", font::mainFont, 16);
		text.setPosition(x*constants::CELL_WIDTH, y*constants::CELL_HEIGHT);
		if(itemIndex == selectedItem) text.setFillColor(colors::brightBlue);
		else text.setFillColor(colors::darkBlue);
		window->draw(text);
		++y;
		++itemIndex;
	}

	std::string creditsString = "credits: " + std::to_string(credits);
	sf::Text creditsText(creditsString, font::mainFont, 16);
	creditsText.setPosition(2*constants::CELL_WIDTH, (y+1)*constants::CELL_HEIGHT);
	creditsText.setFillColor(colors::brightBlue);
	window->draw(creditsText);

	std::string weightString = "weight: " + std::to_string(contentsWeight) + " / " + std::to_string(capacity);
	sf::Text weightText(weightString, font::mainFont, 16);
	weightText.setPosition(2*constants::CELL_WIDTH, (y+2)*constants::CELL_HEIGHT);
	weightText.setFillColor(colors::brightBlue);
	window->draw(weightText);

	std::string commandsString = "(u)se - (d)rop - (w)ield - unwi(e)ld - esc to close";
	sf::Text commandsText(commandsString, font::mainFont, 16);
	commandsText.setPosition(2*constants::CELL_WIDTH, (y+4)*constants::CELL_HEIGHT);
	commandsText.setFillColor(colors::brightBlue);
	window->draw(commandsText);

	window->display();
}

void InventoryMenuState::renderPiles() {
}

void InventoryMenuState::sortIntoPiles() {
	if(inventoryContents.size() > 0) {
		int i = 0;
		while(i < inventoryContents.size()) {
			std::string name = inventoryContents.at(i)->name;
			std::vector<Actor*> pile;
			while(i < inventoryContents.size() && name == inventoryContents.at(i)->name) {
				pile.push_back(inventoryContents.at(i));
				++i;
			}
			piles.push_back(pile);
		}
	}
}
