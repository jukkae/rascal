#include <math.h>
#include "actor.hpp"
#include "ai.hpp"
#include "attacker.hpp"
#include "constants.hpp"
#include "gameplay_state.hpp"


Actor::Actor(int x, int y, int ch, std::string name, sf::Color col, boost::optional<float> energy, bool stairs) :
	x(x), y(y), ch(ch), col(col), name(name), energy(energy),
	blocks(true), fovOnly(true), stairs(stairs), attacker(nullptr), destructible(nullptr), ai(nullptr),
	pickable(nullptr), container(nullptr) {;}

Actor::~Actor() {
}

float Actor::update(GameplayState* state) {
	if(ai) {
		if(actionsQueue.empty()) actionsQueue.push_back(ai->getNextAction(this));
		float actionCost = actionsQueue.front()->getLength();
		bool success = actionsQueue.front()->execute();
		actionsQueue.pop_front();
		if(success) {
			float turnCost = 100.0f * actionCost / ai->speed;
			for(auto& e : statusEffects) {
				e->update(this, state, turnCost);
				if(!e->isAlive()) {
					statusEffects.erase(std::remove(statusEffects.begin(), statusEffects.end(), e), statusEffects.end());
				}
			}
			return turnCost;
		} else {
			if(ai->isPlayer()) {
				return 0;
			} else {
				return constants::DEFAULT_TURN_LENGTH;
			}
		}
	}
	else return constants::DEFAULT_TURN_LENGTH;
}

float Actor::getDistance(int cx, int cy) const {
	int dx = x-cx;
	int dy = y-cy;
	return sqrtf(dx*dx + dy*dy);
}

std::vector<std::unique_ptr<Actor>>& Actor::getActors() {
	return s->getActors();
}


Actor* Actor::getPlayer() {
	for(auto& a : getActors()) {
		if(a->isPlayer()) return a.get();
	}
	return nullptr;
}

Actor* Actor::getClosestMonster(int x, int y, float range) {
	return s->getClosestMonster(x, y, range);
}

Actor* Actor::getLiveActor(int x, int y) {
	return s->getLiveActor(x, y);
}

void Actor::modifyStatistic(Statistic stat, float delta) {
	switch(stat) {
		case Statistic::CONSTITUTION:
			destructible->maxHp += delta;
			destructible->hp += delta;
			break;
		case Statistic::STRENGTH:
			attacker->increase(delta);
			break;
		case Statistic::AGILITY:
			destructible->defense += delta;
			break;
		case Statistic::SPEED:
			ai->speed += delta;
			break;
		default: break;
	}
}
