#include "dialogue_state.hpp"
#include "actor.hpp"
#include "status_effect.hpp"
#include "engine_command.hpp"

DialogueState::DialogueState(Engine* engine, Actor* player, Actor* other) :
State(engine, engine->getWindow()),
player(player),
other(other) {
	console = Console(ConsoleType::NARROW, ClearMode::TRANSPARENT);
	n1.text = "Hello, traveler!";
	n1.replies.push_back({"Eat dirt, scumbag!", nullptr});
	n1.replies.push_back({"Nice to meet you, too!", nullptr});
	n1.replies.push_back({"... [blank stare]", nullptr});
}

void DialogueState::update() {
	handleEvents();
	render();
}

void DialogueState::render() {
  //console.clear(); // TODO what should this function call actually do?
  console.drawGraphicsBlock(Point(16, 3), "Talking with " + other->name, colors::get("brightBlue"));
	console.drawGraphicsBlock(Point(16, 5), "\"" + n1.text + "\"", colors::get("brightBlue"));
	int y = 7;
	int i = 0;
	for(auto& a : n1.replies) {
		sf::Color c = i == selectedReplyIndex ? colors::get("brightBlue") : colors::get("blue");
		console.drawGraphicsBlock(Point(16, y + i), a.first, c);
		++i;
	}
  console.draw();
  window->display();
}

void DialogueState::handleEvents() {
	sf::Event event;
	while(engine->pollEvent(event)) {
		if(event.type == sf::Event::KeyPressed) {
			using k = sf::Keyboard::Key;
			switch(event.key.code) {
				case k::Up:
					if(selectedReplyIndex > 0) --selectedReplyIndex;
					break;
				case k::Down:
					if(selectedReplyIndex < n1.replies.size() - 1) ++selectedReplyIndex;
					break;
				case k::Return:
					//handleItem(menuContents.at(selectedItem));
					engine->addEngineCommand(ContinueCommand(engine));
					break;
				default:
					break;
			}
		}
	}
}
