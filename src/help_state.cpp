#include "help_state.hpp"

#include "colors.hpp"
#include "constants.hpp"
#include "engine_command.hpp"
#include "io.hpp"
#include "version.hpp"

HelpState::HelpState(Engine* engine) :
State(engine, engine->getWindow())
{
	{
	std::ifstream fin("assets/help.txt");
	std::stringstream buffer;
	buffer << fin.rdbuf();
	helpText = buffer.str();
	}
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

	int lines;
	lines = std::count(helpText.begin(), helpText.end(), '\n');
	x = (constants::SCREEN_WIDTH - 24) / 2; //FIXME known width
	io::text(helpText, x, 6, colors::lightGreen);

	std::string story = "To escape, fight your way to the top...";
	x = (constants::SCREEN_WIDTH - story.length()) / 2;
	io::text(story, x, 6 + lines + 3, colors::brightBlue);


	std::string copyright = "(c) 2017- jukkae";
	x = (constants::SCREEN_WIDTH - copyright.length()) / 2;
	io::text(copyright, x, constants::SCREEN_HEIGHT - 3, colors::brightBlue);
}

