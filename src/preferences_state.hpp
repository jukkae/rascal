#ifndef PREFERENCES_STATE_HPP
#define PREFERENCES_STATE_HPP
#include "state.hpp"

struct Preferences;
class PreferencesState : public State {
public:
	PreferencesState(Engine* engine);

	void handleEvents() override;
	void update() override;
	void render() override;
private:
	struct PreferenceItem {
		std::string& key;
		bool& value; //FIXME support other types for values too
	};
	std::vector<PreferenceItem> items;
	int selectedItem = 0;

	Preferences& preferences;
	std::string music;
	std::string dummy;

	void handleSelectedItem();
};

#endif /* PREFERENCES_STATE_HPP */
