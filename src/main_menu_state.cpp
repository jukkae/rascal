#include "main_menu_state.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "engine.hpp"
#include "engine_command.hpp"
#include "font.hpp"
#include "help_state.hpp"
#include "io.hpp"
#include "preferences_state.hpp"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <fstream>

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
	//window->clear(sf::Color::Black);
	console.clear();

	handleEvents();
	render();

	console.draw();
	window->display();
	// TODO where would the call to SFML window display() belong?
	// It's intimately tied to each scene's own update loop, so it can't be moved to, for example, Engine
	// - at least, not straightforward
	// ConsoleStack for each scene, and Stack would then call that?
	// Seems needlessly convoluted
	// Have State call that (like now?) – seems dirty
}

void MainMenuState::render() {
	renderBgArt();
	renderAsciiTitle();
	showMenu();
}

void MainMenuState::renderAsciiTitle() {
	int x = (constants::SCREEN_WIDTH - 41) / 2; // title is 41 cells wide
	int y = 1;
	console.drawGraphicsBlock(Point(x, y), asciiTitle, colors::get("brightBlue"));
}

void MainMenuState::renderBgArt() {
	int x = 0; // bg is hardcoded
	int y = 0;
	console.drawGraphicsBlock(Point(x, y), bgArt, colors::get("lightGreen"));
}

void MainMenuState::showMenu() {
	int menuX = 10;
	int menuY = constants::SCREEN_HEIGHT / 3;

	int itemIndex = 0;
	for(MenuItem item : menuItems) {
		menuX = (constants::SCREEN_WIDTH - item.label.length()) / 2;
		sf::Color color = selectedItem == itemIndex ? colors::get("brightBlue") : colors::get("darkBlue");
		console.drawGraphicsBlock(Point(menuX, menuY+itemIndex*3), item.label, color);
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
