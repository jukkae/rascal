#include "gameover_state.hpp"
#include "actor.hpp"
#include "ai.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "main_menu_state.hpp"
#include <iostream>
#include <sys/stat.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Text.hpp>

GameOverState::GameOverState(Actor* actor) :
	actor(actor)
	{
		if(!font.loadFromFile("assets/FSEX300.ttf")) {
			std::cout << "error loading font\n";
		} else std::cout << "font loaded!\n";

		description = "you died at level ";
		description.append(std::to_string(((PlayerAi*)actor->ai.get())->xpLevel));
		struct stat buffer;
		if(stat (constants::SAVE_FILE_NAME.c_str(), &buffer) == 0) { // If file exists
			// TODO delete
		}
	}

void GameOverState::init(Engine* engine) {
}

void GameOverState::cleanup() {
}

void GameOverState::handleEvents(Engine* engine) {
	sf::Event event;
	while(engine->pollEvent(event)) {
		if(event.type == sf::Event::KeyPressed) {
			using k = sf::Keyboard::Key;
			switch(event.key.code) {
				case k::Return: {
					State* mainMenuState = new MainMenuState();
					mainMenuState->init(engine);
					changeState(engine, mainMenuState);
					break;
				}
				default: break;
			}
		}
	}
}

void GameOverState::update(Engine* engine, sf::RenderWindow& window) {
	handleEvents(engine);
	render(engine, window);
}

void GameOverState::render(Engine* engine, sf::RenderWindow& window) {
	window.clear(sf::Color::Black);
	sf::Text text(description, font, 16);
	text.setFillColor(colors::brightBlue);
	window.draw(text);
	window.display();
}
