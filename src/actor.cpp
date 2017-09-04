#include "actor.hpp"
#include "ai.hpp"
#include "attacker.hpp"
#include "constants.hpp"
#include "gameplay_state.hpp"


Actor::Actor(int x, int y, int ch, std::string name, const TCODColor& col, boost::optional<float> energy) :
	x(x), y(y), ch(ch), col(col), name(name), energy(energy),
	blocks(true), fovOnly(true), attacker(nullptr), destructible(nullptr), ai(nullptr),
	pickable(nullptr), container(nullptr) {;}

Actor::~Actor() {
}

float Actor::update(GameplayState* state) {
	if(ai) {
		if(actionsQueue.empty()) actionsQueue.push(ai->getNextAction(this));
		float actionCost = actionsQueue.front()->getLength();
		bool success = actionsQueue.front()->execute();
		actionsQueue.pop();
		if(success) {
			float turnCost = 100.0f * actionCost / ai->speed;
			return turnCost;
		} else return 0;
	}
	else return constants::DEFAULT_TURN_LENGTH; // ai-less actors should not get turns in the first place
	// actually, maybe they should: for example, decaying materials etc!
	//return ai ? ai->update(this, state) : DEFAULT_TURN_LENGTH;
}

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
