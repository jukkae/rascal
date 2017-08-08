#ifndef MAIN_MENU_STATE_HPP
#define MAIN_MENU_STATE_HPP
#include "state.hpp"
#include <iostream>
class Engine;
class MainMenuState : public State {
public:
	void init() override;
	void cleanup() override;

	void update(Engine* engine) override;
	void render(Engine* engine) override;
private:
	void showMenu(Engine* engine);
};
#endif /* MAIN_MENU_STATE_HPP */
