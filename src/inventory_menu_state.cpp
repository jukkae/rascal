#include <iostream>
#include <algorithm>
#include "inventory_menu_state.hpp"
#include "engine.hpp"
#include "engine_command.hpp"
#include "action.hpp"
#include "actor.hpp"
#include "body.hpp"
#include "body_part.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "effect.hpp"
#include "font.hpp"
#include "io.hpp"
#include "status_effect.hpp"

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
						if(piles.at(selectedItem).at(0) == actor->wornWeapon) {
							actor->addAction(std::make_unique<UnWieldItemAction>(UnWieldItemAction(actor, piles.at(selectedItem).at(0))));
							engine->addEngineCommand(ContinueCommand(engine));
						}
						//else if(piles.at(selectedItem).at(0) == actor->wornArmor) {
						else if(actor->wornArmors.end() != std::find_if(actor->wornArmors.begin(), actor->wornArmors.end(), [&](auto& a){ return piles.at(selectedItem).at(0) == a; })) {
							actor->addAction(std::make_unique<UnWieldItemAction>(UnWieldItemAction(actor, piles.at(selectedItem).at(0))));
							engine->addEngineCommand(ContinueCommand(engine));
						} else {
							actor->addAction(std::make_unique<WieldItemAction>(WieldItemAction(actor, piles.at(selectedItem).at(0))));
							engine->addEngineCommand(ContinueCommand(engine));
						}
					}
					break;
				case k::E:
						actor->addAction(std::make_unique<EatAction>(actor, piles.at(selectedItem).at(0)));
						engine->addEngineCommand(ContinueCommand(engine));
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

	io::text("I N V E N T O R Y", 2, 1, colors::brightBlue);

	int x = 2;
	int y = 3;

	int itemIndex = 0;
	for (auto pile : piles) {
		sf::Color color = itemIndex == selectedItem ? colors::brightBlue : colors::darkBlue;
		io::text(pile.at(0)->name + " (" + std::to_string(pile.size()) + ")", x, y, color);
		++y;
		++itemIndex;
	}

	std::string creditsString = "credits: " + std::to_string(credits);
	io::text(creditsString, 2, y+1, colors::brightBlue);

	std::string weightString = "weight: " + std::to_string(contentsWeight) + " / " + std::to_string(capacity);
	io::text(weightString, 2, y+2, colors::brightBlue);

	std::string commandsString = "(u)se - (d)rop - (w)ield/un(w)ield - (e)at - esc to close";
	io::text(commandsString, 2, y+4, colors::brightBlue);

	renderStats();
	renderBodyParts();

	window->display();
}

void InventoryMenuState::renderStats() {

	io::text("S T A T S", 70, 1, colors::brightBlue);

	int x = 64;
	int y = 3;

	if(actor->body) {
		Body* b = actor->body.get();

		std::string strength = "    Strength: " + std::to_string(b->strength);
		io::text(strength, x, y+1, colors::brightBlue);

		std::string perception = "  Perception: " + std::to_string(b->perception);
		io::text(perception, x, y+2, colors::brightBlue);

		std::string endurance = "   Endurance: " + std::to_string(b->endurance);
		io::text(endurance, x, y+3, colors::brightBlue);

		std::string charisma = "    Charisma: " + std::to_string(b->charisma);
		io::text(charisma, x, y+4, colors::brightBlue);

		std::string intelligence = "Intelligence: " + std::to_string(b->intelligence);
		io::text(intelligence, x, y+5, colors::brightBlue);

		std::string agility = "     Agility: " + std::to_string(b->agility);
		io::text(agility, x, y+6, colors::brightBlue);

		std::string luck = "        Luck: " + std::to_string(b->luck);
		io::text(luck, x, y+7, colors::brightBlue);

		std::string speed = "       Speed: " + std::to_string(b->speed);
		io::text(speed, x, y+8, colors::brightBlue);

	} else {
		std::string no = "You have no body. How about that?";
		io::text(no, x, y+1, colors::brightBlue);
	}
}

void InventoryMenuState::renderBodyParts() {
	io::text("B O D Y", 90, 1, colors::brightBlue);

	int x = 90;
	int y = 3;

	if(actor->body) {
		Body* b = actor->body.get();

		auto freeBodyParts = b->bodyParts;

		std::string bps = "body parts:";
		io::text(bps, x, y+1, colors::brightBlue);

		for(auto& bp : freeBodyParts) {
			++y;
			std::string bpt = "";
			switch(bp.first) {
				case BodyPart::HAND_L: bpt.append("left hand");  break;
				case BodyPart::HAND_R: bpt.append("right hand"); break;
				case BodyPart::FEET  : bpt.append("feet");       break;
				case BodyPart::TORSO : bpt.append("torso");      break;
				case BodyPart::HEAD  : bpt.append("head");       break;
				default: break;
			};
			sf::Color col;
			if(bp.second) col = colors::brightBlue;
			else col = colors::blue;
			io::text(bpt, x, y+1, col);
		}
	}
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
