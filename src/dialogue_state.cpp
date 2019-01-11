#include "dialogue_state.hpp"
#include "actor.hpp"
#include "status_effect.hpp"
#include "engine_command.hpp"

DialogueState::DialogueState(Engine* engine, Actor* player, Actor* other) :
State(engine, engine->getWindow()),
player(player),
other(other) {
	console = Console(ConsoleType::NARROW, ClearMode::BLACK); // TODO would prefer TRANSPARENT, but that's broken

	n0.text = "Hello, traveler!";
	n0.replies.push_back({"Eat dirt, scumbag!", &n1});
	n0.replies.push_back({"Nice to meet you, too!", &n2});
	n0.replies.push_back({"... [blank stare]", &n3});

	n1.text = "That wasn't very nice of you.";
	n1.replies.push_back({"... [blank stare]", nullptr});

	n2.text = "Say, what do you think of the weather?";
	n2.replies.push_back({"It's bad.", &n3});
	n2.replies.push_back({"It's good.", &n3});
	n2.replies.push_back({"It's okay.", &n3});

	n3.text = "I'm sorry to have wasted your time. See you!";
	n3.replies.push_back({"... [blank stare]", nullptr});
}

void DialogueState::update() {
	handleEvents();
	render();
}

void DialogueState::render() {
  console.clear(); // TODO what should this function call actually do?
  console.drawGraphicsBlock(Point(16, 3), "Talking with " + other->name, colors::get("brightBlue"));
	console.drawGraphicsBlock(Point(16, 5), "\"" + currentNode->text + "\"", colors::get("brightBlue"));
	int y = 7;
	int i = 0;
	for(auto& a : currentNode->replies) {
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
					if(selectedReplyIndex < currentNode->replies.size() - 1) ++selectedReplyIndex;
					break;
				case k::Return:
					if(currentNode->replies.at(selectedReplyIndex).second != nullptr) {
						//console.clear();
						currentNode = currentNode->replies.at(selectedReplyIndex).second;
						selectedReplyIndex = 0;
					}
					else engine->addEngineCommand(ContinueCommand(engine));
					break;
				case k::Escape:
					engine->addEngineCommand(ContinueCommand(engine));
					break;
				default:
					break;
			}
		}
	}
}
