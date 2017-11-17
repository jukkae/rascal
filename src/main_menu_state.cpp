#include "main_menu_state.hpp"
#include "constants.hpp"
#include "engine.hpp"

#include <sys/stat.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Text.hpp>

static const sf::Color brightBlue(100, 100, 255);
static const sf::Color darkBlue(0, 0, 155);


void MainMenuState::init(Engine* engine) {
	if(!font.loadFromFile("assets/FSEX300.ttf")) {
		std::cout << "error loading font\n";
	} else std::cout << "font loaded!\n";
	std::cout << font.getInfo().family << "\n";

	MenuItem newGame = { MenuItemCode::NEW_GAME, "New game!" };
	MenuItem cont = { MenuItemCode::CONTINUE, "Continue!" };
	MenuItem exit = { MenuItemCode::EXIT, "Exit!" };
	menuItems.push_back(newGame);

	struct stat buffer;
	if(stat (constants::SAVE_FILE_NAME.c_str(), &buffer) == 0) { // If file exists
		menuItems.push_back(cont);
	}

	menuItems.push_back(exit);

	selectedItem = 0;
}

void MainMenuState::cleanup() {

}

void MainMenuState::handleEvents(Engine* engine) {
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
					handleSelectedMenuItem(engine);
					break;
				default:
					break;
			}
		}
	}
}

void MainMenuState::update(Engine* engine, sf::RenderWindow& window) {
	window.clear(sf::Color::Black);

	handleEvents(engine);
	render(engine, window);

	window.display();
}

void MainMenuState::render(Engine* engine, sf::RenderWindow& window) {
	showMenu(engine, window);
}

void MainMenuState::showMenu(Engine* engine, sf::RenderWindow& window) {
	int menuX = 10;
	int menuY = constants::SCREEN_HEIGHT / 3;

	int itemIndex = 0;
	for(MenuItem item : menuItems) {
		sf::Text itemText(item.label, font, 16);
		if (selectedItem == itemIndex) {
			itemText.setColor(brightBlue);
		} else {
			itemText.setColor(darkBlue);
		}

		itemText.setPosition(menuX * constants::CELL_WIDTH, (menuY + itemIndex * 3) * constants::CELL_HEIGHT);
		window.draw(itemText);
		++itemIndex;
	}
}

void MainMenuState::handleSelectedMenuItem(Engine* engine) {
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
