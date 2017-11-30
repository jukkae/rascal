#include "status_effect.hpp"
#include "colors.hpp"
#include "destructible.hpp"
#include "gameplay_state.hpp"

void TestStatusEffect::update(Actor* owner, GameplayState* state, float deltaTime) {
	time -= deltaTime;
	std::cout << "test status\n";
	std::cout << "time elapsed: " << deltaTime << "\n";
	std::cout << "time left: " << time << "\n";
}

bool TestStatusEffect::isAlive() {
	return time > 0;
}

void PoisonedStatusEffect::update(Actor* owner, GameplayState* state, float deltaTime) {
	time -= deltaTime;
	for(int i = 0; i < deltaTime; ++i) {
		++counter;
		if(counter % interval == 0) {
			counter = 0;
			owner->destructible->takeDamage(owner, damage);
			owner->s->message(colors::red, "you take damage from the poison!");
		}
	}
}

bool PoisonedStatusEffect::isAlive() {
	return time > 0;
}

BOOST_CLASS_EXPORT_IMPLEMENT(TestStatusEffect)
BOOST_CLASS_EXPORT_IMPLEMENT(PoisonedStatusEffect)
