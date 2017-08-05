#ifndef MAIN_MENU_STATE_HPP
#define MAIN_MENU_STATE_HPP
class MainMenuState : public State {
public:
	void init() override {;}
	void cleanup() override {;}

	void update(Engine* engine) override {std::cout << "Main menu update\n";}
	void render(Engine* engine) override {std::cout << "Main menu render\n";}

}
#endif /* MAIN_MENU_STATE_HPP */
