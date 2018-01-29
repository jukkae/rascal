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
						if(piles.at(selectedItem).at(0) == actor->wornWeapon || piles.at(selectedItem).at(0) == actor->wornArmor) {
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

	std::string commandsString = "(u)se - (d)rop - (w)ield/un(w)ield - esc to close";
	sf::Text commandsText(commandsString, font::mainFont, 16);
	commandsText.setPosition(2*constants::CELL_WIDTH, (y+4)*constants::CELL_HEIGHT);
	commandsText.setFillColor(colors::brightBlue);
	window->draw(commandsText);

	renderStats();
	renderBodyParts();

	window->display();
}

void InventoryMenuState::renderStats() {

	sf::Text heading("S T A T S", font::mainFont, 16);
	heading.setPosition(70*constants::CELL_WIDTH, constants::CELL_HEIGHT);
	heading.setFillColor(colors::brightBlue);
	window->draw(heading);

	int x = 64;
	int y = 3;

	if(actor->body) {
		Body* b = actor->body.get();

		std::string strength = "    Strength: " + std::to_string(b->strength);
		sf::Text strengthText(strength, font::mainFont, 16);
		strengthText.setPosition(x*constants::CELL_WIDTH, (y+1)*constants::CELL_HEIGHT);
		strengthText.setFillColor(colors::brightBlue);
		window->draw(strengthText);

		std::string perception = "  Perception: " + std::to_string(b->perception);
		sf::Text perceptionText(perception, font::mainFont, 16);
		perceptionText.setPosition(x*constants::CELL_WIDTH, (y+2)*constants::CELL_HEIGHT);
		perceptionText.setFillColor(colors::brightBlue);
		window->draw(perceptionText);

		std::string endurance = "   Endurance: " + std::to_string(b->endurance);
		sf::Text enduranceText(endurance, font::mainFont, 16);
		enduranceText.setPosition(x*constants::CELL_WIDTH, (y+3)*constants::CELL_HEIGHT);
		enduranceText.setFillColor(colors::brightBlue);
		window->draw(enduranceText);

		std::string charisma = "    Charisma: " + std::to_string(b->charisma);
		sf::Text charismaText(charisma, font::mainFont, 16);
		charismaText.setPosition(x*constants::CELL_WIDTH, (y+4)*constants::CELL_HEIGHT);
		charismaText.setFillColor(colors::brightBlue);
		window->draw(charismaText);

		std::string intelligence = "Intelligence: " + std::to_string(b->intelligence);
		sf::Text intelligenceText(intelligence, font::mainFont, 16);
		intelligenceText.setPosition(x*constants::CELL_WIDTH, (y+5)*constants::CELL_HEIGHT);
		intelligenceText.setFillColor(colors::brightBlue);
		window->draw(intelligenceText);

		std::string agility = "     Agility: " + std::to_string(b->agility);
		sf::Text agilityText(agility, font::mainFont, 16);
		agilityText.setPosition(x*constants::CELL_WIDTH, (y+6)*constants::CELL_HEIGHT);
		agilityText.setFillColor(colors::brightBlue);
		window->draw(agilityText);

		std::string luck = "        Luck: " + std::to_string(b->luck);
		sf::Text luckText(luck, font::mainFont, 16);
		luckText.setPosition(x*constants::CELL_WIDTH, (y+7)*constants::CELL_HEIGHT);
		luckText.setFillColor(colors::brightBlue);
		window->draw(luckText);

		std::string speed = "       Speed: " + std::to_string(b->speed);
		sf::Text speedText(speed, font::mainFont, 16);
		speedText.setPosition(x*constants::CELL_WIDTH, (y+8)*constants::CELL_HEIGHT);
		speedText.setFillColor(colors::brightBlue);
		window->draw(speedText);

	} else {
		std::string no = "You have no body. How about that?";
		sf::Text noText(no, font::mainFont, 16);
		noText.setPosition(x*constants::CELL_WIDTH, (y+1)*constants::CELL_HEIGHT);
		noText.setFillColor(colors::brightBlue);
		window->draw(noText);
	}
}

void InventoryMenuState::renderBodyParts() {
	sf::Text heading("B O D Y", font::mainFont, 16);
	heading.setPosition(90*constants::CELL_WIDTH, constants::CELL_HEIGHT);
	heading.setFillColor(colors::brightBlue);
	window->draw(heading);

	int x = 90;
	int y = 3;

	if(actor->body) {
		Body* b = actor->body.get();

		auto freeBodyParts = b->getFreeBodyParts();

		std::string bps = "free body parts: " + std::to_string(freeBodyParts.size());
		sf::Text bpsText(bps, font::mainFont, 16);
		bpsText.setPosition(x*constants::CELL_WIDTH, (y+1)*constants::CELL_HEIGHT);
		bpsText.setFillColor(colors::brightBlue);
		window->draw(bpsText);

		for(auto& bp : freeBodyParts) {
			++y;
			std::string bpt = "";
			switch(bp) {
				case BodyPart::HAND_L: bpt.append("left hand");  break;
				case BodyPart::HAND_R: bpt.append("right hand"); break;
				case BodyPart::FOOT_L: bpt.append("left foot");  break;
				case BodyPart::FOOT_R: bpt.append("right foot"); break;
				case BodyPart::TORSO : bpt.append("torso");      break;
				case BodyPart::HEAD  : bpt.append("head");       break;
				default: break;
			};
			sf::Text bpText(bpt, font::mainFont, 16);
			bpText.setPosition(x*constants::CELL_WIDTH, (y+1)*constants::CELL_HEIGHT);
			bpText.setFillColor(colors::brightBlue);
			window->draw(bpText);
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
