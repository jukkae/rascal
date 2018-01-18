#include "gameover_state.hpp"
#include "actor.hpp"
#include "ai.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "effect.hpp"
#include "font.hpp"
#include "io.hpp"
#include "main_menu_state.hpp"
#include "player.hpp"
#include "status_effect.hpp"
#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Text.hpp>

GameOverState::GameOverState(Engine* engine, Actor* actor, Player* player, bool victory) :
State(engine, engine->getWindow())
{
	if(!victory) {
		description = "you died at level ";
		description.append(std::to_string(((PlayerAi*)actor->ai.get())->xpLevel));
		description.append("\n");
		description.append("score: ");
		description.append(std::to_string(player->score));
		if(io::fileExists(constants::SAVE_FILE_NAME)) {
			io::removeFile(constants::SAVE_FILE_NAME);
		}
	} else {
		description = "you won at level ";
		description.append(std::to_string(((PlayerAi*)actor->ai.get())->xpLevel));
		if(io::fileExists(constants::SAVE_FILE_NAME)) {
			io::removeFile(constants::SAVE_FILE_NAME);
		}
	}
}

void GameOverState::handleEvents() {
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

void GameOverState::update() {
	window->clear(sf::Color::Black);

	handleEvents();
	render();

	window->display();
}

void GameOverState::render() {
	int x = 2;
	int y = 2;
	sf::Text text(description, font::mainFont, 16);
	text.setPosition(x * constants::CELL_WIDTH, y * constants::CELL_HEIGHT);
	text.setFillColor(colors::brightBlue);
	window->draw(text);
}
