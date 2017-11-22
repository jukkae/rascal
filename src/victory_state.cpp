#include "victory_state.hpp"
#include "main_menu_state.hpp"
#include "font.hpp"
#include "colors.hpp"
#include <sys/stat.h>

VictoryState::VictoryState(Actor* actor) :
actor(actor)
{
	description = "you won at level ";
	description.append(std::to_string(((PlayerAi*)actor->ai.get())->xpLevel));
	struct stat buffer;
	if(stat (constants::SAVE_FILE_NAME.c_str(), &buffer) == 0) { // If file exists
		remove(constants::SAVE_FILE_NAME.c_str());
	}

}

void VictoryState::init(Engine* engine) { }

void VictoryState::cleanup() { }

void VictoryState::handleEvents(Engine* engine) {
	sf::Event event;
	while(engine->pollEvent(event)) {
		if(event.type == sf::Event::KeyPressed) {
			using k = sf::Keyboard::Key;
			switch(event.key.code) {
				case k::Return: {
					std::unique_ptr<State> mainMenuState = std::make_unique<MainMenuState>();
					mainMenuState->init(engine);
					changeState(engine, std::move(mainMenuState));
					break;
				}
				default: break;
			}
		}
	}
}

void VictoryState::update(Engine* engine, sf::RenderWindow& window) {
	handleEvents(engine);
	render(engine, window);
}

void VictoryState::render(Engine* engine, sf::RenderWindow& window) {
	window.clear(sf::Color::Black);
	sf::Text text(description, font::mainFont, 16);
	text.setFillColor(colors::brightBlue);
	window.draw(text);
	window.display();
}
