#include "dialogue_state.hpp"
#include "engine_command.hpp"

DialogueState::DialogueState(Engine* engine, Actor* actor) :
State(engine, engine->getWindow()),
actor(actor) {
	console = Console(ConsoleType::NARROW);
}

void DialogueState::update() {
	handleEvents();
	render();
}

void DialogueState::render() {
  console.clear();
  console.drawGraphicsBlock(Point(2, 1), "DIALOGUE", colors::get("brightBlue"));
  console.draw();
  window->display();
}

void DialogueState::handleEvents() {
  sf::Event event;
  while(engine->pollEvent(event)) {
    if(event.type == sf::Event::KeyPressed) {
			using k = sf::Keyboard::Key;
			switch(event.key.code) {
        case k::Escape:
					engine->addEngineCommand(ContinueCommand(engine));
					break;
        default: break;
      }
    }
  }
}
