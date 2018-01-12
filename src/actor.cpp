#include <cmath>
#include "actor.hpp"
#include "ai.hpp"
#include "attacker.hpp"
#include "body.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "damage.hpp"
#include "destructible.hpp"
#include "effect.hpp"
#include "event.hpp"
#include "gameplay_state.hpp"
#include "pickable.hpp"
#include "status_effect.hpp"
#include "transporter.hpp"
#include "world.hpp"


Actor::Actor(int x, int y, int ch, std::string name, sf::Color col, boost::optional<float> energy) :
	x(x), y(y), ch(ch), col(col), name(name), energy(energy),
	blocks(true), fovOnly(true), attacker(nullptr), destructible(nullptr), ai(nullptr),
	pickable(nullptr), container(nullptr), transporter(nullptr) {;}

Actor::~Actor() {
}

float Actor::update(GameplayState* state) {
	if(ai) {
		float fovRadius = constants::DEFAULT_FOV_RADIUS * (body ? body->perception / 10.0 : 1.0);
		if(isPlayer()) world->computeFov(x, y, fovRadius);
		if(actionsQueue.empty()) actionsQueue.push_back(ai->getNextAction(this));
		float actionCost = actionsQueue.front()->getLength();
		bool success = actionsQueue.front()->execute();
		actionsQueue.pop_front();
		if(success) {
			float turnCost = 100.0f * actionCost / ai->speed;
			if(body) turnCost = turnCost * 10.0f / body->agility;
			for(auto& e : statusEffects) {
				e->update(this, state, turnCost);
				if(!e->isAlive()) {
					statusEffects.erase(std::remove(statusEffects.begin(), statusEffects.end(), e), statusEffects.end());
				}
			}
			if(isPlayer()) world->computeFov(x, y, fovRadius);
			return turnCost;
		} else {
			if(ai->isPlayer()) {
				if(isPlayer()) world->computeFov(x, y, fovRadius);
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

void Actor::modifyStatistic(Statistic stat, int delta) {
	if(body) {
		switch(stat) {
			case Statistic::STRENGTH:
				body->strength += delta;
				break;
			case Statistic::PERCEPTION:
				body->perception += delta;
				break;
			case Statistic::ENDURANCE:
				body->endurance += delta;
				break;
			case Statistic::CHARISMA:
				body->charisma += delta;
				break;
			case Statistic::INTELLIGENCE:
				body->intelligence += delta;
				break;
			case Statistic::AGILITY:
				body->agility += delta;
				break;
			case Statistic::LUCK:
				body->luck += delta;
				break;
			default: break;
		}
	}
}

bool Actor::tryToMove(Direction direction, float distance) {
	if(distance <= 0) return false;

	while(distance > 0) {
		int targetX = x;
		int targetY = y;

		switch(direction) {
			case Direction::N:  targetY -= 1;               break;
			case Direction::NE: targetY -= 1; targetX += 1; break;
			case Direction::E:                targetX += 1; break;
			case Direction::SE: targetY += 1; targetX += 1; break;
			case Direction::S:  targetY += 1;               break;
			case Direction::SW: targetY += 1; targetX -= 1; break;
			case Direction::W:                targetX -= 1; break;
			case Direction::NW: targetY -= 1; targetX -= 1; break;
			case Direction::NONE: return false;
			default: break;
		}

		if (world->isWall(targetX, targetY)) {
			destructible->takeDamage(this, distance, DamageType::CRUSHING);
			GenericActorEvent e(this, "%s gets crushed against the wall!");
			world->notify(e);
			return false;
		}

		if (world->getLiveActor(targetX, targetY)) {
			GenericActorEvent e(this, "%s gently bumps into someone.");
			world->notify(e);
			return false;
		}
		x = targetX;
		y = targetY;

		--distance;
	}
	return true;
}

bool Actor::isPlayer() { return ai ? this->ai->isPlayer() : false; }

int Actor::getAC() {
	int ac = 0;
	if(body) ac = 10 + body->getModifier(body->agility);
	if(wornArmor) ++ac; //FIXME
	return ac;
}
