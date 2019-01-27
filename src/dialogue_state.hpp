#ifndef DIALOGUE_STATE_HPP
#define DIALOGUE_STATE_HPP
#include "state.hpp"
#include <SFML/Graphics.hpp>

struct DialogueGraphNode {
	std::string text;
	std::vector<std::pair<std::string, DialogueGraphNode*>> replies;
};

class Engine;
class Actor;
class DialogueState : public State {
public:
	DialogueState(Engine* engine, Actor* player, Actor* other);

	void handleEvents() override;
	void update() override;
	void render() override;
private:
	int selectedReplyIndex = 0;
	DialogueGraphNode n0, n1, n2, n3;
	DialogueGraphNode* currentNode = &n0;
	Actor* player;
	Actor* other;
};

#endif /* DIALOGUE_STATE_HPP */
