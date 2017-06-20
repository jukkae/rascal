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

void Actor::update() {
	if(ai) ai->update(this);
}

float Actor::getDistance(int cx, int cy) const {
	int dx = x-cx;
	int dy = y-cy;
	return sqrtf(dx*dx + dy*dy);
}
