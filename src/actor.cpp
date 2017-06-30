#include "actor.hpp"
#include "ai.hpp"
#include "attacker.hpp"
#include "pickable.hpp"

static const float DEFAULT_TURN_LENGTH = 100; // I know, this is now in two places

Actor::Actor(int x, int y, int ch, std::string name, const TCODColor& col) :
	x(x), y(y), ch(ch), col(col), name(name),
	blocks(true), fovOnly(true), attacker(nullptr), destructible(nullptr), ai(nullptr),
	pickable(nullptr), container(nullptr) {;}

Actor::~Actor() {
}

void Actor::render() const {
	TCODConsole::root->setChar(x, y, ch);
	TCODConsole::root->setCharForeground(x, y, col);
}

float Actor::update() {
	return ai ? ai->update(this) : DEFAULT_TURN_LENGTH;
}

float Actor::getDistance(int cx, int cy) const {
	int dx = x-cx;
	int dy = y-cy;
	return sqrtf(dx*dx + dy*dy);
}
