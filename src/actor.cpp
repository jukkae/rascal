#include <stdio.h>
#include <math.h>
#include "main.hpp"

Actor::Actor(int x, int y, int ch, std::string name, const TCODColor& col) :
	x(x), y(y), ch(ch), col(col), name(name),
	blocks(true), fovOnly(true), attacker(NULL), destructible(NULL), ai(NULL),
	pickable(NULL), container(NULL) {;}

Actor::Actor() : // TODO dirty hack
	x(0), y(0), ch(1), col(TCODColor::white), name(""),
	blocks(true), fovOnly(true), attacker(NULL), destructible(NULL), ai(NULL),
	pickable(NULL), container(NULL) {;}

Actor::~Actor() {
	if(ai) delete ai;
}

void Actor::render() const {
	TCODConsole::root->setChar(x, y, ch);
	TCODConsole::root->setCharForeground(x, y, col);
}

int Actor::update() {
	if(ai) return ai->update(this);
	else return 100; // TODO yeah this won't work in the long run
}

float Actor::getDistance(int cx, int cy) const {
	int dx = x-cx;
	int dy = y-cy;
	return sqrtf(dx*dx + dy*dy);
}
