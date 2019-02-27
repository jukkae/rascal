#ifndef DIALOGUE_STATE_HPP
#define DIALOGUE_STATE_HPP
#include "state.hpp"
#include <SFML/Graphics.hpp>

class Engine;
class Actor;

class DialogueAction {
public:
	DialogueAction(Actor* player, Actor* other, bool createMission = false) : player(player), other(other), createMission(createMission) {}
	Actor* player;
	Actor* other;
	bool createMission;
	void execute();
};

struct DialogueGraphNode {
	int id;
	std::string text;
	std::vector<std::pair<std::string, std::optional<int>>> replies;
	bool createMission = false;
	DialogueAction enter(Actor* player, Actor* other) { return DialogueAction{ player, other, createMission }; }
};

class DialogueState : public State {
public:
	DialogueState(Engine* engine, Actor* player, Actor* other);

	void handleEvents() override;
	void update() override;
	void render() override;
private:
	void initializeDialogueGraph();
	std::vector<DialogueGraphNode> dialogueGraph;
	int selectedReplyIndex = 0;
	DialogueGraphNode* currentNode = nullptr;
	Actor* player;
	Actor* other;
};

#endif /* DIALOGUE_STATE_HPP */
