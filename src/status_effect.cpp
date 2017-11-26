#include "status_effect.hpp"

void StatusEffect::update(Actor* owner, GameplayState* state, float deltaTime) {
	std::cout << "test status\n";
	std::cout << "time elapsed: " << deltaTime << "\n";
}
