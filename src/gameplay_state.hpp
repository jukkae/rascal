#ifndef GAMEPLAY_STATE_HPP
#define GAMEPLAY_STATE_HPP
class Engine;
#include "state.hpp"
class GameplayState : public State {
public:
	void init() override;
	void cleanup() override;

	void update(Engine* engine) override;
	void render(Engine* engine) override;
};
#endif /* GAMEPLAY_STATE_HPP */
