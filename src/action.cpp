#include "action.hpp"
#include "actor.hpp"
#include "attacker.hpp"
#include "gameplay_state.hpp"

bool MoveAction::execute() {
	GameplayState* state = actor->s;
	int targetX = actor->x;
	int targetY = actor->y;

	switch(direction) {
		case Direction::N:  targetY -= 1;                break;
		case Direction::NE: targetY -= 1; targetX += 1; break;
		case Direction::E:                targetX += 1; break;
		case Direction::SE: targetY += 1; targetX += 1; break;
		case Direction::S:  targetY += 1;                break;
		case Direction::SW: targetY += 1; targetX -= 1; break;
		case Direction::W:                targetX -= 1; break;
		case Direction::NW: targetY -= 1; targetX -= 1; break;
		case Direction::NONE: break;
		default: break;
	}

	if (state->isWall(targetX, targetY)) return false;

	// look for living actors to attack
	for (Actor* a : *actor->getActors()) {
		if (a->destructible && !a->destructible->isDead() && a->x == targetX && a->y == targetY) {
			actor->attacker->attack(actor, a);
			return false;
		}
	}
	// look for corpses or items
	for (Actor* a : *actor->getActors()) {
		bool corpseOrItem = (a->destructible && a->destructible->isDead()) || a->pickable;
		if(corpseOrItem && a->x == targetX && a->y == targetY) {
			state->message(TCODColor::lightGrey, "There's a %s here!", a->name.c_str());
		}
	}
	actor->x = targetX;
	actor->y = targetY;
	return true;
}
