#include "main_menu_state.hpp"
#include "libtcod.hpp"

void MainMenuState::init() {

}

void MainMenuState::cleanup() {

}

void MainMenuState::update(Engine* engine) {
	std::cout << "Main menu update\n";
	showMenu(engine);
}

void MainMenuState::render(Engine* engine) {
	std::cout << "main menu render\n";
}

void MainMenuState::showMenu(Engine* engine) {
	// TODO have *own* console that's drawn on top of root and, you know, do something sane
	TCODConsole::root->clear();
	TCODConsole::root->flush();
}
