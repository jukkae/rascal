#include "help_state.hpp"

#include "colors.hpp"
#include "constants.hpp"
#include "engine_command.hpp"
#include "io.hpp"
#include "version.hpp"

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
				case k::Escape:
					engine->addEngineCommand(ContinueCommand(engine));
					break;
				case k::Return:
					engine->addEngineCommand(ContinueCommand(engine));
					break;
				default:
					break;
			}
		}
	}
}

void HelpState::update() {
	window->clear(sf::Color::Black);

	handleEvents();
	render();

	window->display();
}

void HelpState::render() {
	std::string header = "R A S C A L";
	int x = (constants::SCREEN_WIDTH - header.length()) / 2;
	io::text(header, x, 1, colors::brightBlue);
	std::string version = "Version " + version::VERSION;
	x = (constants::SCREEN_WIDTH - version.length()) / 2;
	io::text(version, x, 2, colors::brightBlue);
}

