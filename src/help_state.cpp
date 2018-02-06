#include "help_state.hpp"

#include "colors.hpp"
#include "constants.hpp"

HelpState::HelpState(Engine* engine) :
State(engine, engine->getWindow())
{

}

void HelpState::handleEvents() {
	sf::Event event;
	while(engine->pollEvent(event)) {
		if(event.type == sf::Event::KeyPressed) {
			using k = sf::Keyboard::Key;
			switch(event.key.code) {
				case k::Up:
					//if(selectedItem > 0) --selectedItem;
					break;
				case k::Down:
					//if(selectedItem < menuItems.size() - 1) ++selectedItem;
					break;
				case k::Return:
					//handleSelectedMenuItem();
					break;
				default:
					break;
			}
		}
	}
}

void HelpState::update() {
	window->clear(sf::Color::Black);

	render();

	window->display();
}

void HelpState::render() {

}

