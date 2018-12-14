#include "help_state.hpp"

#include "colors.hpp"
#include "constants.hpp"
#include "engine_command.hpp"
#include "io.hpp"
#include "version.hpp"

HelpState::HelpState(Engine* engine) :
State(engine, engine->getWindow())
{
	console = Console(ConsoleType::NARROW);
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
	console.clear();

	handleEvents();
	render();

	console.draw();
	window->display();
}

void HelpState::render() {
	std::string header = "R A S C A L";
	int x = (constants::SCREEN_WIDTH - header.length()) / 2;
	console.drawGraphicsBlock(Point(x, 1), header, colors::get("brightBlue"));

	std::string version = "Version " + version::VERSION;
	x = (constants::SCREEN_WIDTH - version.length()) / 2;
	console.drawGraphicsBlock(Point(x, 2), version, colors::get("brightBlue"));

	int lines;
	lines = std::count(helpText.begin(), helpText.end(), '\n');
	x = (constants::SCREEN_WIDTH - 24) / 2; //FIXME known width
	console.drawGraphicsBlock(Point(x, 6), helpText, colors::get("lightGreen"));

	std::string story = "To escape, fight your way to the top...";
	x = (constants::SCREEN_WIDTH - story.length()) / 2;
	console.drawGraphicsBlock(Point(x, 6 + lines + 3), story, colors::get("brightBlue"));


	std::string copyright = "(c) 2017- jukkae";
	x = (constants::SCREEN_WIDTH - copyright.length()) / 2;
	console.drawGraphicsBlock(Point(x, constants::SCREEN_HEIGHT - 3), copyright, colors::get("brightBlue"));
}
