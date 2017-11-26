#include "status_effect.hpp"

void TestStatusEffect::update(Actor* owner, GameplayState* state, float deltaTime) {
	time -= deltaTime;
	std::cout << "test status\n";
	std::cout << "time elapsed: " << deltaTime << "\n";
	std::cout << "time left: " << time << "\n";
}

BOOST_CLASS_EXPORT_IMPLEMENT(TestStatusEffect)
