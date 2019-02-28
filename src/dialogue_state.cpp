#include "dialogue_state.hpp"
#include "actor.hpp"
#include "status_effect.hpp"
#include "engine_command.hpp"

void DialogueAction::execute() {
	if(createMission) {
		switch(other->dialogueGenerator->missionType.value()) {
			case MissionType::KILL: {
				auto m = std::make_unique<KillMission>("Kill Bill", "Find and kill Bill");
				other->dialogueGenerator->mission = m.get();
				m->status = MissionStatus::ACTIVE;
				player->missions.push_back(std::move(m));
			} break;
			case MissionType::ACQUIRE_ITEMS: {
				auto m = std::make_unique<AcquireItemsMission>("Acquire RAM", "Acquire 2 RAM chips");
				other->dialogueGenerator->mission = m.get();
				m->status = MissionStatus::ACTIVE;
				player->missions.push_back(std::move(m));
			} break;
			default: break;
		}
	}
	if(finishMission) {
		other->dialogueGenerator->mission->status = MissionStatus::COMPLETED;
	}
}

DialogueState::DialogueState(Engine* engine, Actor* player, Actor* other) :
State(engine, engine->getWindow()),
player(player),
other(other) {
	console = Console(ConsoleType::NARROW, ClearMode::BLACK); // TODO would prefer TRANSPARENT, but that's broken

	initializeDialogueGraph();
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
					if(currentNode->replies.at(selectedReplyIndex).second != std::nullopt) {
						//console.clear();
						currentNode = &dialogueGraph.at(currentNode->replies.at(selectedReplyIndex).second.value());
						DialogueAction da = currentNode->enter(player, other);
						da.execute();
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

void DialogueState::initializeDialogueGraph() {
	using std::nullopt;
	DialogueGraphNode n0, n1, n2, n3, n4;
	if(other->dialogueGenerator->missionType) {
		switch(other->dialogueGenerator->missionType.value()) {
		case MissionType::KILL: {
			if(!other->dialogueGenerator->mission) {
				n0.text = "Hello, traveler!";
				n0.replies.push_back({"Eat dirt, scumbag!", 1});
				n0.replies.push_back({"Nice to meet you, too!", 2});
				n0.replies.push_back({"... [blank stare]", 3});

				n1.text = "You sound like a tough one. I need a certain Bill dead.";
				n1.replies.push_back({"Sounds good. What's in it for me?", 4});
				n1.replies.push_back({"Naw man, that ain't me.", nullopt});
				n1.replies.push_back({"... [blank stare]", nullopt});

				n2.text = "Say, what do you think of the weather?";
				n2.replies.push_back({"Heck off, old man.", 1});
				n2.replies.push_back({"It's bad.", 3});
				n2.replies.push_back({"It's good.", 3});
				n2.replies.push_back({"It's okay.", 3});

				n3.text = "I'm sorry to have wasted your time. See you!";
				n3.replies.push_back({"... [blank stare]", nullopt});

				n4.createMission = true;
				n4.text = "Besides the joy of killing? Money. Go at it.";
				n4.replies.push_back({"... [blank stare]", nullopt});

				dialogueGraph.push_back(n0);
				dialogueGraph.push_back(n1);
				dialogueGraph.push_back(n2);
				dialogueGraph.push_back(n3);
				dialogueGraph.push_back(n4);
			} else {
				switch(other->dialogueGenerator->mission->status) {
				case MissionStatus::ACTIVE: {
					n0.text = "He's dead?";
					n0.replies.push_back({"He ain't dead yet.", 1});

					n1.text = "Well, do something about it.";
					n1.replies.push_back({"I'm on it.", nullopt});

					dialogueGraph.push_back(n0);
					dialogueGraph.push_back(n1);
				} break;
				case MissionStatus::REQUIRES_CONFIRMATION: {
					n0.text = "He's dead?";
					n0.replies.push_back({"He's dead.", 1});

					n1.finishMission = true;
					n1.text = "That's just lovely.";
					n1.replies.push_back({"The money?", nullopt});

					dialogueGraph.push_back(n0);
					dialogueGraph.push_back(n1);
				} break;
				case MissionStatus::COMPLETED: {
					n0.text = "How did you like it?";
					n0.replies.push_back({"I loved it! Aaah!", nullopt});
					dialogueGraph.push_back(n0);
				} break;
				default: break;
				}
			}
		} break;
		case MissionType::ACQUIRE_ITEMS: {
			n0.text = "I'm in dire need of some RAM chips.";
			n0.replies.push_back({"Tough.", nullopt});
			n0.replies.push_back({"How many do you need?", 1});

			n1.text = "Two packs. I'll give you 25 credits.";
			n1.replies.push_back({"It's a deal.", 2});
			n1.replies.push_back({"Too low.", nullopt});

			n2.createMission = true;
			n2.text = "I'll wait for you here.";
			n2.replies.push_back({"... [blank stare]", nullopt});

			dialogueGraph.push_back(n0);
			dialogueGraph.push_back(n1);
			dialogueGraph.push_back(n2);
		} break;
		default: break;
		}
	} else { // no missionType
		n0.text = "I don't know you.";
		n0.replies.push_back({"That makes two of us.", nullopt});

		dialogueGraph.push_back(n0);
	}
	currentNode = &dialogueGraph.at(0);
}
