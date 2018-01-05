#include "status_effect.hpp"
#include "colors.hpp"
#include "destructible.hpp"
#include "event.hpp"
#include "gameplay_state.hpp"

void PoisonedStatusEffect::update(Actor* owner, GameplayState* state, float deltaTime) {
	time -= deltaTime;
	for(int i = 0; i < deltaTime; ++i) {
		++counter;
		if(counter % interval == 0) {
			counter = 0;
			StatusEffectEvent e(owner, "you take damage from the poison!");
			owner->world->notify(e);
			owner->destructible->takeDamage(owner, damage);
		}
	}
}

bool PoisonedStatusEffect::isAlive() {
	return time > 0;
}

BOOST_CLASS_EXPORT_IMPLEMENT(PoisonedStatusEffect)
