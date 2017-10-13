#include "ai.hpp"
#include "action.hpp"
#include "actor.hpp"
#include "attacker.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "engine.hpp"
#include "gameplay_state.hpp"
#include "gui.hpp"
#include "input_handler.hpp"
#include "map.hpp"
#include "pickable.hpp"
#include "persistent.hpp"
#include "state.hpp"
#include "level_up_menu_state.hpp"
#include <math.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include "boost/optional.hpp"

static const int TRACKING_TURNS = 3;

const int LEVEL_UP_BASE = 50;
const int LEVEL_UP_FACTOR = 150;

int PlayerAi::getNextLevelXp() const {
	return LEVEL_UP_BASE + xpLevel * LEVEL_UP_FACTOR;
}

void PlayerAi::increaseXp(int xp, GameplayState* state) {
	experience += xp;
	if(experience >= getNextLevelXp()) {
		experience -= getNextLevelXp();
		++xpLevel;
		// TODO message(TCODColor::yellow, "You've reached level %d!", xpLevel);
		Engine* engine = state->getEngine();
		Actor* player = state->getPlayer();
		State* levelUpMenuState = new LevelUpMenuState(player);
		levelUpMenuState->init(engine);
		engine->pushState(levelUpMenuState);
	}
}

Action* PlayerAi::getNextAction(Actor* actor) {
	Direction dir = Direction::NONE;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;

	boost::optional<RawInputEvent> event = actor->s->inputHandler->getEvent(actor);
	if(!event) {
		return new EmptyAction(actor);
	}
	lastKey = event->key;

	switch(lastKey.vk) {
		case TCODK_UP: {
			dir = Direction::N;
			break;
		}
		case TCODK_DOWN: {
			dir = Direction::S;
			break;
		}
		case TCODK_LEFT: {
			dir = Direction::W;
			break;
		}
		case TCODK_RIGHT: {
			dir = Direction::E;
			break;
		}
		case TCODK_CHAR: {
			switch(lastKey.c) {
				case 'g': {
					return new PickupAction(actor);
				}
				case -89: {
					if(lastKey.shift) {
						std::cout << "go down\n";
						// go down
					}
					else {
						std::cout << "go up\n";
						// go up
					}
				}
			}
		}
		default: dir = Direction::NONE; break;
	}
	return new MoveAction(actor, dir); // TODO news leak memory currently
}

Action* MonsterAi::getNextAction(Actor* actor) {
	GameplayState* state = actor->s;
	Direction direction = Direction::NONE;
	if (actor->destructible && actor->destructible->isDead()) return new WaitAction(actor);

	if (actor->s->isInFov(actor->x, actor->y)) {
		moveCount = TRACKING_TURNS;
	} else { --moveCount; }

	if (moveCount > 0) {
		Actor* player = state->getPlayer();
		int targetX = player->x;
		int targetY = player->y;
		int dx = targetX - actor->x;
		int dy = targetY - actor->y;
		int stepDx = (dx == 0 ? 0 : (dx > 0 ? 1 : -1));
		int stepDy = (dy == 0 ? 0 : (dy > 0 ? 1 : -1));
		float distance = sqrtf(dx*dx + dy*dy);

		if(distance >= 2) {
			state->message(TCODColor::white, "The %s threatens you!", actor->name.c_str());
			dx = (int) (round(dx / distance));
			dy = (int) (round(dy / distance));
			if(state->canWalk(actor->x + stepDx, actor->y + stepDy)) { // uhh
				if (stepDx ==  0 && stepDy ==  0) return new WaitAction(actor);

				if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
				if (stepDx ==  1 && stepDy == -1) direction = Direction::NE;
				if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
				if (stepDx ==  1 && stepDy ==  1) direction = Direction::SE;
				if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
				if (stepDx == -1 && stepDy ==  1) direction = Direction::SW;
				if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
				if (stepDx == -1 && stepDy == -1) direction = Direction::NW;
				return new MoveAction(actor, direction);
			} else if (state->canWalk(actor->x + stepDx, actor->y)) { // Wall sliding
				if (stepDx ==  0 && stepDy ==  0) return new WaitAction(actor);
				if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
				if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
				return new MoveAction(actor, direction);
			} else if (state->canWalk(actor->x, actor->y + stepDy)) {
				if (stepDx ==  0 && stepDy ==  0) return new WaitAction(actor);
				if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
				if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
				return new MoveAction(actor, direction);
			}
		} else { // Melee range
			if (stepDx ==  0 && stepDy ==  0) return new WaitAction(actor);

			if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
			if (stepDx ==  1 && stepDy == -1) direction = Direction::NE;
			if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
			if (stepDx ==  1 && stepDy ==  1) direction = Direction::SE;
			if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
			if (stepDx == -1 && stepDy ==  1) direction = Direction::SW;
			if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
			if (stepDx == -1 && stepDy == -1) direction = Direction::NW;
			return new MoveAction(actor, direction);
		}
	}
	return new WaitAction(actor);
}

/* float TemporaryAi::update(Actor* owner, GameplayState* state) { // TODO reimplement
	--turns;
	if(turns <= 0) {
		owner->ai = std::move(oldAi);
	}
	return 100 * (100 / speed);
} */

void TemporaryAi::applyTo(Actor* actor) {
	oldAi = std::move(actor->ai);
	actor->ai = std::unique_ptr<Ai>(this);
}

/* float ConfusedMonsterAi::update(Actor* owner, GameplayState* state) { // TODO reimplement
	TCODRandom* rng = TCODRandom::getInstance();
	int dx = rng->getInt(-1,1);
	int dy = rng->getInt(-1,1);
	if(dx != 0 || dy != 0) {
		int destX = owner->x + dx;
		int destY = owner->y + dy;
		if(state->canWalk(destX, destY)) {
			owner->x = destX;
			owner->y = destY;
		} else {
			Actor* target = state->getLiveActor(destX, destY);
			if(target) { owner->attacker->attack(owner, target); }
		}
	}
	return TemporaryAi::update(owner, state);
} */

BOOST_CLASS_EXPORT(PlayerAi)
BOOST_CLASS_EXPORT(MonsterAi)
BOOST_CLASS_EXPORT(TemporaryAi)
BOOST_CLASS_EXPORT(ConfusedMonsterAi)
