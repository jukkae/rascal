#include "main_menu_state.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "engine.hpp"
#include "engine_command.hpp"
#include "font.hpp"
#include "io.hpp"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Text.hpp>

MainMenuState::MainMenuState(Engine* engine, bool forceShowContinue) :
State(engine, engine->getWindow())
{
	MenuItem newGame = { MenuItemCode::NEW_GAME, "New game!" };
	MenuItem cont = { MenuItemCode::CONTINUE, "Continue!" };
	MenuItem exit = { MenuItemCode::EXIT, "Exit!" };

	menuItems.push_back(newGame);

	if(io::fileExists(constants::SAVE_FILE_NAME) || forceShowContinue) {
		if(!engine->gameOver) menuItems.push_back(cont);
		}
	menuItems.push_back(exit);

	selectedItem = 0;

	std::ifstream fin("assets/asciiTitle.txt");
	std::stringstream buffer;
	buffer << fin.rdbuf();
	asciiTitle = buffer.str();
}

void MainMenuState::handleEvents() {
	sf::Event event;
	while(engine->pollEvent(event)) {
		if(event.type == sf::Event::KeyPressed) {
			using k = sf::Keyboard::Key;
			switch(event.key.code) {
				case k::Up:
					if(selectedItem > 0) --selectedItem;
					break;
				case k::Down:
					if(selectedItem < menuItems.size() - 1) ++selectedItem;
					break;
				case k::Return:
					handleSelectedMenuItem();
					break;
				default:
					break;
			}
		}
	}
}

void MainMenuState::update() {
	window->clear(sf::Color::Black);

	handleEvents();
	render();

	window->display();
}

void MainMenuState::render() {
	renderAsciiTitle();
	showMenu();
}

void MainMenuState::renderAsciiTitle() {
	int x = 1;
	int y = 1;
	sf::Text text(asciiTitle, font::mainFont, 16);
	text.setPosition(x * constants::CELL_WIDTH, y * constants::CELL_HEIGHT);
	text.setFillColor(colors::brightBlue);
	window->draw(text);
}

void MainMenuState::showMenu() {
	int menuX = 10;
	int menuY = constants::SCREEN_HEIGHT / 3;

	int itemIndex = 0;
	for(MenuItem item : menuItems) {
		sf::Text itemText(item.label, font::mainFont, 16);
		if (selectedItem == itemIndex) {
			itemText.setFillColor(colors::brightBlue);
		} else {
			itemText.setFillColor(colors::darkBlue);
		}

		itemText.setPosition(menuX * constants::CELL_WIDTH, (menuY + itemIndex * 3) * constants::CELL_HEIGHT);
		window->draw(itemText);
		++itemIndex;
	}
}

void MainMenuState::handleSelectedMenuItem() {
	switch (menuItems.at(selectedItem).code) {
		case MenuItemCode::NEW_GAME:
			engine->addEngineCommand(NewGameCommand(engine));
			break;
		case MenuItemCode::CONTINUE:
			engine->addEngineCommand(ContinueCommand(engine));
			break;
		case MenuItemCode::EXIT:
			engine->addEngineCommand(ExitCommand(engine));
			break;
		case MenuItemCode::NONE:
			break;
		default:
			break;
	}
}
