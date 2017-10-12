#include "gameover_state.hpp"
#include <iostream>

void GameOverState::init(Engine* engine) {
	console.setDefaultBackground(TCODColor::black);
}

void GameOverState::cleanup() {

}

void GameOverState::handleEvents(Engine* engine) {
	TCOD_key_t key;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr);

	switch (key.vk) {
		case TCODK_ENTER:
			// TODO
			break;
		default:
			break;
	}

}

void GameOverState::update(Engine* engine) {

}

void GameOverState::render(Engine* engine) {
	console.setDefaultForeground(TCODColor::lightGrey);
	console.print(0, 0, "you're dead");
	console.flush();
	TCODConsole::blit(&console, 0, 0, constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, TCODConsole::root, 0, 0);
}
