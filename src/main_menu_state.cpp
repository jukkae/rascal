#include "main_menu_state.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "engine.hpp"
#include "engine_command.hpp"
#include "font.hpp"
#include "help_state.hpp"
#include "io.hpp"
#include "preferences_state.hpp"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

MainMenuState::MainMenuState(Engine* engine, bool forceShowContinue) :
State(engine, engine->getWindow())
{
	console = Console(ConsoleType::NARROW);
	MenuItem newGame = { MenuItemCode::NEW_GAME, "New game!" };
	MenuItem cont = { MenuItemCode::CONTINUE, "Continue!" };
	MenuItem help = { MenuItemCode::HELP, "Help & about" };
	MenuItem preferences = { MenuItemCode::PREFERENCES, "Preferences" };
	MenuItem exit = { MenuItemCode::EXIT, "Exit!" };

	menuItems.push_back(newGame);

	if(io::fileExists(constants::SAVE_FILE_NAME) || forceShowContinue) {
		if(!engine->gameOver) menuItems.push_back(cont);
	}
	menuItems.push_back(help);
	menuItems.push_back(preferences);
	menuItems.push_back(exit);

	selectedItem = 0;

	{
	std::ifstream fin("assets/asciiTitle.txt");
	std::stringstream buffer;
	buffer << fin.rdbuf();
	asciiTitle = buffer.str();
	}

	{
	std::ifstream fin("assets/bgArt.txt");
	std::stringstream buffer;
	buffer << fin.rdbuf();
	bgArt = buffer.str();
	}
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

	console.draw();
	window->display();
}

void MainMenuState::render() {
	renderBgArt();
	renderAsciiTitle();
	showMenu();
}

void MainMenuState::renderAsciiTitle() {
	int x = (constants::SCREEN_WIDTH - 41) / 2; // title is 41 cells wide
	int y = 1;
	console.drawText(Point(x, y), asciiTitle, colors::brightBlue);
}

void MainMenuState::renderBgArt() {
	int x = 0; // bg is hardcoded
	int y = 0;
	io::text(bgArt, x, y, colors::lightGreen);
}

void MainMenuState::showMenu() {
	int menuX = 10;
	int menuY = constants::SCREEN_HEIGHT / 3;

	int itemIndex = 0;
	for(MenuItem item : menuItems) {
		menuX = (constants::SCREEN_WIDTH - item.label.length()) / 2;
		sf::Color color = selectedItem == itemIndex ? colors::brightBlue : colors::darkBlue;
		io::text(item.label, menuX, menuY+itemIndex*3, color);
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
		case MenuItemCode::HELP:
			{
				std::unique_ptr<State> helpState = std::make_unique<HelpState>(engine);
				engine->pushState(std::move(helpState));
			}
			break;
		case MenuItemCode::PREFERENCES:
			{
				std::unique_ptr<State> preferencesState = std::make_unique<PreferencesState>(engine);
				engine->pushState(std::move(preferencesState));
			}
			break;
		case MenuItemCode::NONE:
			break;
		default:
			break;
	}
}
