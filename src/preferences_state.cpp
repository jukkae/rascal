#include "preferences_state.hpp"

#include "colors.hpp"
#include "constants.hpp"
#include "engine_command.hpp"
#include "io.hpp"
#include "version.hpp"

PreferencesState::PreferencesState(Engine* engine) :
State(engine, engine->getWindow())
{
	{
	std::ifstream fin("assets/preferences.txt");
	std::stringstream buffer;
	buffer << fin.rdbuf();
	preferences = buffer.str();
	}
}

void PreferencesState::handleEvents() {
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

void PreferencesState::update() {
	window->clear(sf::Color::Black);

	handleEvents();
	render();

	window->display();
}

void PreferencesState::render() {
	std::string header = "P R E F E R E N C E S";
	int x = (constants::SCREEN_WIDTH - header.length()) / 2;
	io::text(header, x, 1, colors::brightBlue);

	int lines;
	lines = std::count(preferences.begin(), preferences.end(), '\n');
	x = (constants::SCREEN_WIDTH - preferences.length()) / 2; //FIXME known width
	io::text(preferences, x, 6, colors::brightBlue);
}
