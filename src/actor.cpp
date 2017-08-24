#include "actor.hpp"
#include "ai.hpp"
#include "attacker.hpp"
#include "gameplay_state.hpp"

static const float DEFAULT_TURN_LENGTH = 100; // I know, this is now in two places

Actor::Actor(int x, int y, int ch, std::string name, const TCODColor& col, boost::optional<float> energy) :
	x(x), y(y), ch(ch), col(col), name(name), energy(energy),
	blocks(true), fovOnly(true), attacker(nullptr), destructible(nullptr), ai(nullptr),
	pickable(nullptr), container(nullptr) {;}

Actor::~Actor() {
}

float Actor::update(GameplayState* state) {
	if(ai) {
		if(actionsQueue.empty()) actionsQueue.push(ai->getNextAction(this));
		actionsQueue.front()->execute();
		actionsQueue.pop();
	}
	return ai ? ai->update(this, state) : DEFAULT_TURN_LENGTH;
}

/* TODO FIXME URGENT and NEXT UP and all that jazz: new implementation gist:
   Actor::update() {
	if (actionsQueue.isEmpty()) actionsQueue.add(ai->getNewAction());
	actionsQueue[0].execute();
	actionsQueue.pop_first();
   }
   In this model, MonsterAi just generates Actions just as it basically does now.
   PlayerAi will request kbd input from inputHandler, and will block until inputHandler responds.
*/

float Actor::getDistance(int cx, int cy) const {
	int dx = x-cx;
	int dy = y-cy;
	return sqrtf(dx*dx + dy*dy);
}

std::vector<Actor*>* Actor::getActors() { return s->getActors(); }

Actor* Actor::getPlayer() {
	for(auto a : *getActors()) {
		if(a->isPlayer()) return a;
	}
	return nullptr;
}

Actor* Actor::getClosestMonster(int x, int y, float range) {
	return s->getClosestMonster(x, y, range);
}

Actor* Actor::getLiveActor(int x, int y) {
	return s->getLiveActor(x, y);
}
