#include "player.hpp"

#include "event.hpp"
#include "ignore.hpp"

void Player::notify(Event& event) {
	if(auto e = dynamic_cast<PlayerStatChangeEvent*>(&event)) {
		ignore(e);
		score += 10;
	}
	if(auto e = dynamic_cast<DeathEvent*>(&event)) {
		ignore(e);
		score += 1;
	}
}
