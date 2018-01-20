#include "player.hpp"

#include "event.hpp"
#include "ignore.hpp"

void Player::notify(Event& event) {
	if(auto e = dynamic_cast<PlayerStatChangeEvent*>(&event)) {
		score += 10;
	}
}
