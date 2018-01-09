#include "victory_state.hpp"
#include "actor.hpp"
#include "main_menu_state.hpp"
#include "font.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "io.hpp"

VictoryState::VictoryState(Engine* engine, Actor* actor) :
State(engine, engine->getWindow())
{
	description = "you won at level ";
	description.append(std::to_string(((PlayerAi*)actor->ai.get())->xpLevel));
	description.append("\n");
	description.append("you had ");
	int numberOfMacGuffins = 0;
	for(auto& i : actor->container->inventory) { if (i->name == "phlebotinum link") numberOfMacGuffins++; }
	description.append(std::to_string(numberOfMacGuffins));
	description.append(" phlebotinum links");

	if(io::fileExists(constants::SAVE_FILE_NAME)) { // If file exists
		io::removeFile(constants::SAVE_FILE_NAME);
	}

}

void VictoryState::handleEvents() {
	sf::Event event;
	while(engine->pollEvent(event)) {
		if(event.type == sf::Event::KeyPressed) {
			using k = sf::Keyboard::Key;
			switch(event.key.code) {
				case k::Return: {
					std::unique_ptr<State> mainMenuState = std::make_unique<MainMenuState>(engine, window);
					changeState(std::move(mainMenuState));
					break;
				}
				default: break;
			}
		}
	}
}

void VictoryState::update() {
	handleEvents();
	render();
}

void VictoryState::render() {
	int x = 2;
	int y = 2;
	window->clear(sf::Color::Black);
	sf::Text text(description, font::mainFont, 16);
	text.setPosition(x * constants::CELL_WIDTH, y * constants::CELL_HEIGHT);
	text.setFillColor(colors::brightBlue);
	window->draw(text);
	window->display();
}
