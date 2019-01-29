#ifndef DIALOGUE_STATE_HPP
#define DIALOGUE_STATE_HPP
#include "state.hpp"
#include <SFML/Graphics.hpp>

class Engine;
class Actor;

class DialogueAction {
public:
	DialogueAction(Actor* player, bool createMission = false) : player(player), createMission(createMission) {}
	Actor* player;
	bool createMission;
	void execute();
};

struct DialogueGraphNode {
	std::string text;
	std::vector<std::pair<std::string, DialogueGraphNode*>> replies;
	bool createMission = false;
	DialogueAction enter(Actor* player) { return DialogueAction{ player, createMission }; }
};

class DialogueState : public State {
public:
	DialogueState(Engine* engine, Actor* player, Actor* other);

	void handleEvents() override;
	void update() override;
	void render() override;
private:
	int selectedReplyIndex = 0;
	DialogueGraphNode n0, n1, n2, n3, n4;
	DialogueGraphNode* currentNode = &n0;
	Actor* player;
	Actor* other;
};

#endif /* DIALOGUE_STATE_HPP */
