#include "preferences_state.hpp"

#include "colors.hpp"
#include "constants.hpp"
#include "engine_command.hpp"
#include "io.hpp"
#include "preferences.hpp"
#include "version.hpp"

PreferencesState::PreferencesState(Engine* engine) :
State(engine, engine->getWindow()),
preferences(engine->preferences)
{
	console = Console(ConsoleType::NARROW);
	PreferenceItem music = { preferences.music.first, preferences.music.second };
	//PreferenceItem dummy = { preferences.dummy.first, preferences.dummy.second };
	//PreferenceItem longer = { preferences.longer_dummy_preference.first, preferences.longer_dummy_preference.second };
	items.push_back(music);
	//items.push_back(dummy);
	//items.push_back(longer);
}

void PreferencesState::handleEvents() {
	sf::Event event;
	while(engine->pollEvent(event)) {
		if(event.type == sf::Event::KeyPressed) {
			using k = sf::Keyboard::Key;
			switch(event.key.code) {
				case k::Up:
					if(selectedItem > 0) --selectedItem;
					break;
				case k::Down:
					if(selectedItem < items.size() - 1) ++selectedItem;
					break;
				case k::Left:
					items.at(selectedItem).value ^= true;
					if(items.at(selectedItem).key == "music") {
						switch(engine->audioPlayer.music.getStatus()) {
							case sf::SoundSource::Status::Stopped: {
								engine->audioPlayer.music.play();
								break;
							}
							case sf::SoundSource::Status::Paused: {
								engine->audioPlayer.music.play();
								break;
							}
							case sf::SoundSource::Status::Playing: {
								engine->audioPlayer.music.stop(); // Stop or pause?
								break;
							}
							default: break;
						}
					}
					break;
				case k::Right:
					items.at(selectedItem).value ^= true;
					if(items.at(selectedItem).key == "music") {
						switch(engine->audioPlayer.music.getStatus()) {
							case sf::SoundSource::Status::Stopped: {
								engine->audioPlayer.music.play();
								break;
							}
							case sf::SoundSource::Status::Paused: {
								engine->audioPlayer.music.play();
								break;
							}
							case sf::SoundSource::Status::Playing: {
								engine->audioPlayer.music.stop(); // Stop or pause?
								break;
							}
							default: break;
						}
					}
					break;
				case k::Escape:
					engine->addEngineCommand(ContinueCommand(engine));
					break;
				case k::Return:
					handleSelectedItem();
					break;
				default:
					break;
			}
		}
	}
}

void PreferencesState::update() {
	console.clear();

	handleEvents();
	render();

	console.draw();
	window->display();
}

void PreferencesState::render() {
	std::string header = "P R E F E R E N C E S";
	int x = (constants::SCREEN_WIDTH - header.length()) / 2;
	console.drawGraphicsBlock(Point(x, 1), header, colors::get("brightBlue"));


	int menuY = 4;
	int itemIndex = 0;
	for(PreferenceItem item : items) {
		x = (constants::SCREEN_WIDTH / 2) - item.key.length(); // center colons
		sf::Color color = selectedItem == itemIndex ? colors::get("brightBlue") : colors::get("darkBlue");
		console.drawGraphicsBlock(Point(x, menuY+itemIndex), item.key + " : " + (item.value ? "on" : "off"), color);
		++itemIndex;
	}
}

void PreferencesState::handleSelectedItem() {

}
