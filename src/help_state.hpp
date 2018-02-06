#ifndef HELP_STATE_HPP
#define HELP_STATE_HPP
#include "state.hpp"

class HelpState : public State {
public:
	HelpState(Engine* engine);

	void handleEvents() override;
	void update() override;
	void render() override;
private:
};

#endif /* HELP_STATE_HPP */
