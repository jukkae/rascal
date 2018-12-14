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
	console = Console(ConsoleType::NARROW);
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

void InventoryMenuState::sortIntoPiles() {
	if(inventoryContents.size() > 0) {
		int i = 0;
		while(i < inventoryContents.size()) {
			std::string name = inventoryContents.at(i)->name;
			std::vector<Actor*> pile;
			while(i < inventoryContents.size() && name == inventoryContents.at(i)->name) {
				pile.push_back(inventoryContents.at(i));
				++i;
				if(pile.back()->rangedAttacker) break;
				if(pile.back()->attacker) break;
				if(pile.back()->armor) break; // FIXME proper sorting for all sorts of wieldables: make them stack again
			}
			piles.push_back(pile);
		}
	}
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
				case k::T:
					if(inventoryContents.size() > 0) {
						actor->addAction(std::make_unique<ThrowItemAction>(actor, piles.at(selectedItem).at(0)));
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
							if(!piles.at(selectedItem).at(0)->attacker && !piles.at(selectedItem).at(0)->rangedAttacker && !piles.at(selectedItem).at(0)->armor) break;
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
	//window->clear(sf::Color::Black);
	console.clear();
	console.drawGraphicsBlock(Point(2, 1), "I N V E N T O R Y", colors::get("brightBlue"));

	int x = 2;
	int y = 3;

	int itemIndex = 0;
	for (auto pile : piles) {
		sf::Color color = itemIndex == selectedItem ? colors::get("brightBlue") : colors::get("darkBlue");
		int pileSize = pile.size();
		if(pileSize != 1) {
			console.drawGraphicsBlock(Point(x, y), pile.at(0)->name + " (" + std::to_string(pileSize) + ")", color);
		}
		else {
			if(pile.at(0) == actor->wornWeapon || (std::find(actor->wornArmors.begin(), actor->wornArmors.end(), pile.at(0)) != actor->wornArmors.end())) {
				console.drawGraphicsBlock(Point(x, y), pile.at(0)->name + " (worn)", color);
			} else {
				console.drawGraphicsBlock(Point(x, y), pile.at(0)->name, color);
			}
		}
		++y;
		++itemIndex;
	}

	std::string creditsString = "credits: " + std::to_string(credits);
	console.drawGraphicsBlock(Point(2, y+1), creditsString, colors::get("brightBlue"));

	std::string weightString = "weight: " + std::to_string(contentsWeight) + " / " + std::to_string(capacity);
	console.drawGraphicsBlock(Point(2, y+2), weightString, colors::get("brightBlue"));

	std::string commandsString = "(u)se - (d)rop - (w)ield/un(w)ield - (e)at - (t)hrow";
	console.drawGraphicsBlock(Point(2, y+4), commandsString, colors::get("brightBlue"));

	std::string closeString = "esc to close";
	console.drawGraphicsBlock(Point(2, y+5), closeString, colors::get("brightBlue"));

	renderStats();
	renderBodyParts();

	console.draw();
	window->display();

}

void InventoryMenuState::renderStats() {

	console.drawGraphicsBlock(Point(70, 1), "S T A T S", colors::get("brightBlue"));

	int x = 64;
	int y = 3;

	if(actor->body) {
		std::string statsBlock = "";
		Body* b = actor->body.get();

		std::string strength = "    Strength: " + std::to_string(b->strength) + "\n";
		statsBlock.append(strength);

		std::string perception = "  Perception: " + std::to_string(b->perception) + "\n";
		statsBlock.append(perception);

		std::string endurance = "   Endurance: " + std::to_string(b->endurance) + "\n";
		statsBlock.append(endurance);

		std::string charisma = "    Charisma: " + std::to_string(b->charisma) + "\n";
		statsBlock.append(charisma);

		std::string intelligence = "Intelligence: " + std::to_string(b->intelligence) + "\n";
		statsBlock.append(intelligence);

		std::string agility = "     Agility: " + std::to_string(b->agility) + "\n";
		statsBlock.append(agility);

		std::string luck = "        Luck: " + std::to_string(b->luck) + "\n";
		statsBlock.append(luck);

		std::string speed = "       Speed: " + std::to_string(b->speed) + "\n";
		statsBlock.append(speed);

		console.drawGraphicsBlock(Point(x, y), statsBlock, colors::get("brightBlue"));

	} else {
		std::string no = "You have no body. How about that?";
		console.drawGraphicsBlock(Point(x, y+1), no, colors::get("brightBlue"));
	}
}

void InventoryMenuState::renderBodyParts() {
	console.drawGraphicsBlock(Point(90, 1), "B O D Y", colors::get("brightBlue"));

	int x = 90;
	int y = 3;

	if(actor->body) {
		Body* b = actor->body.get();

		auto freeBodyParts = b->bodyParts;

		std::string bps = "body parts:";
		console.drawGraphicsBlock(Point(x, y+1), bps, colors::get("brightBlue"));

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
			if(bp.second) col = colors::get("brightBlue");
			else col = colors::get("blue");
			console.drawGraphicsBlock(Point(x, y+1), bpt, col);
		}
	}
}

void InventoryMenuState::renderPiles() {
}
