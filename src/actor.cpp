#include <math.h>
#include "actor.hpp"
#include "ai.hpp"
#include "attacker.hpp"
#include "constants.hpp"
#include "gameplay_state.hpp"


Actor::Actor(int x, int y, int ch, std::string name, sf::Color col, boost::optional<float> energy) :
	x(x), y(y), ch(ch), col(col), name(name), energy(energy),
	blocks(true), fovOnly(true), attacker(nullptr), destructible(nullptr), ai(nullptr),
	pickable(nullptr), container(nullptr), transporter(nullptr) {;}

Actor::~Actor() {
}

float Actor::update(GameplayState* state) {
	if(ai) {
		if(isPlayer()) world->computeFov(); // Have player request fov computation
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
			if(isPlayer()) world->computeFov(); // Have player request fov computation also after
			return turnCost;
		} else {
			if(ai->isPlayer()) {
				if(isPlayer()) world->computeFov(); // Have player request fov computation
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
	return world->getActors();
}

Actor* Actor::getLiveActor(int x, int y) {
	return world->getLiveActor(x, y);
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
