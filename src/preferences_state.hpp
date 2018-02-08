#ifndef PREFERENCES_STATE_HPP
#define PREFERENCES_STATE_HPP
#include "state.hpp"

class PreferencesState : public State {
public:
	PreferencesState(Engine* engine);

	void handleEvents() override;
	void update() override;
	void render() override;
private:
	std::string preferences;
};

#endif /* PREFERENCES_STATE_HPP */
