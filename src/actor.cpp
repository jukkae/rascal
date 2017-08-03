#include "actor.hpp"
#include "ai.hpp"
#include "attacker.hpp"
#include "engine.hpp"

static const float DEFAULT_TURN_LENGTH = 100; // I know, this is now in two places

Actor::Actor(int x, int y, int ch, std::string name, const TCODColor& col, boost::optional<float> energy) :
	x(x), y(y), ch(ch), col(col), name(name), energy(energy),
	blocks(true), fovOnly(true), attacker(nullptr), destructible(nullptr), ai(nullptr),
	pickable(nullptr), container(nullptr) {;}

Actor::~Actor() {
}

float Actor::update() {
	return ai ? ai->update(this) : DEFAULT_TURN_LENGTH;
}

float Actor::getDistance(int cx, int cy) const {
	int dx = x-cx;
	int dy = y-cy;
	return sqrtf(dx*dx + dy*dy);
}

std::vector<Actor*>* Actor::getActors() { return engine.actors.get(); }
