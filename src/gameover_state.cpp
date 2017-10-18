#include "gameover_state.hpp"
#include "actor.hpp"
#include "ai.hpp"
#include "constants.hpp"
#include "main_menu_state.hpp"
#include "libtcod.hpp"
#include <iostream>

GameOverState::GameOverState(Actor* actor) :
	console(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT),
	actor(actor)
	{
		description = "you died at level ";
		description.append(std::to_string(((PlayerAi*)actor->ai.get())->xpLevel));
		if(TCODSystem::fileExists(constants::SAVE_FILE_NAME.c_str())) { // TODO feels nasty doing this in ctor
			TCODSystem::deleteFile(constants::SAVE_FILE_NAME.c_str());
		}
	}

void GameOverState::init(Engine* engine) {
	console.setDefaultBackground(TCODColor::black);
}

void GameOverState::cleanup() {

}

void GameOverState::handleEvents(Engine* engine) {
	TCOD_key_t key;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr);

	switch (key.vk) {
		case TCODK_ENTER: {
			State* mainMenuState = new MainMenuState();
			mainMenuState->init(engine);
			changeState(engine, mainMenuState);
			break;
		}
		default:
			break;
	}

}

void GameOverState::update(Engine* engine) {
	handleEvents(engine);
	render(engine);
}

void GameOverState::render(Engine* engine) {
	console.setDefaultForeground(TCODColor::lightGrey);
	console.print(0, 0, description.c_str());
	console.flush();
	TCODConsole::blit(&console, 0, 0, constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, TCODConsole::root, 0, 0);
}
