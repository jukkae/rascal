#include "main_menu_state.hpp"
#include "libtcod.hpp"

void MainMenuState::init() {

}

void MainMenuState::cleanup() {

}

void MainMenuState::update(Engine* engine) {
	showMenu(engine);
}

void MainMenuState::render(Engine* engine) {
}

void MainMenuState::showMenu(Engine* engine) {
	console.setDefaultBackground(TCODColor::black);
	console.setDefaultForeground(TCODColor::white);
	console.print(1, 1, "AAAAA");
	console.flush();
	TCODConsole::blit(&console, 0, 0, constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, TCODConsole::root, 0, 0);
}
