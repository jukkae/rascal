#include "actor.hpp"
#include "constants.hpp"
#include "gameplay_state.hpp"
#include "io.hpp"
#include "point.hpp"
#include <SFML/Graphics.hpp>

bool io::waitForMouseClick(GameplayState* state) {
	while(true) {
		sf::Event event;
		while(window.pollEvent(event)) { // Dummy polling to keep macOS happy
			state->render();
			if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				return true;
			}
			if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				return false;
			}
		}
	}
}
