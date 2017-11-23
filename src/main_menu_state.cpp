#include "main_menu_state.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "engine.hpp"
#include "font.hpp"

#include <sys/stat.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Text.hpp>

MainMenuState::MainMenuState(Engine* engine, bool forceShowContinue) :
State(engine, engine->getWindow()),
forceShowContinue(forceShowContinue) {
	MenuItem newGame = { MenuItemCode::NEW_GAME, "New game!" };
	MenuItem cont = { MenuItemCode::CONTINUE, "Continue!" };
	MenuItem exit = { MenuItemCode::EXIT, "Exit!" };

	menuItems.push_back(newGame);

	struct stat buf;
	int result = stat (constants::SAVE_FILE_NAME.c_str(), &buf);
	if(result == 0 || forceShowContinue) { // 
		menuItems.push_back(cont);
	}
	menuItems.push_back(exit);

	selectedItem = 0;
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
	showMenu();
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
