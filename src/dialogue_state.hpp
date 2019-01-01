#ifndef DIALOGUE_STATE_HPP
#define DIALOGUE_STATE_HPP
#include "state.hpp"
#include <SFML/Graphics.hpp>

class Engine;
class Actor;
class DialogueState : public State {
public:
	DialogueState(Engine* engine, Actor* actor);

	void handleEvents() override;
	void update() override;
	void render() override;
private:
  Actor* actor;
};

#endif /* DIALOGUE_STATE_HPP */
