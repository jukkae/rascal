#include "victory_state.hpp"
#include "main_menu_state.hpp"
#include "font.hpp"
#include "colors.hpp"
#include <sys/stat.h>

VictoryState::VictoryState(Engine* engine, Actor* actor) :
State(engine),
actor(actor)
{
	description = "you won at level ";
	description.append(std::to_string(((PlayerAi*)actor->ai.get())->xpLevel));
	struct stat buffer;
	if(stat (constants::SAVE_FILE_NAME.c_str(), &buffer) == 0) { // If file exists
		remove(constants::SAVE_FILE_NAME.c_str());
	}

}

void VictoryState::handleEvents() {
	sf::Event event;
	while(engine->pollEvent(event)) {
		if(event.type == sf::Event::KeyPressed) {
			using k = sf::Keyboard::Key;
			switch(event.key.code) {
				case k::Return: {
					std::unique_ptr<State> mainMenuState = std::make_unique<MainMenuState>(engine);
					changeState(std::move(mainMenuState));
					break;
				}
				default: break;
			}
		}
	}
}

void VictoryState::update(sf::RenderWindow& window) {
	handleEvents();
	render(window);
}

void VictoryState::render(sf::RenderWindow& window) {
	window.clear(sf::Color::Black);
	sf::Text text(description, font::mainFont, 16);
	text.setFillColor(colors::brightBlue);
	window.draw(text);
	window.display();
}
