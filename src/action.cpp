#include "action.hpp"
#include "actor.hpp"
#include "attacker.hpp"
#include "gameplay_state.hpp"

bool MoveAction::execute() {
	// TODO behavior can also be chosen upon actor->ai->faction
	GameplayState* state = actor->s;
	int targetX = actor->x;
	int targetY = actor->y;

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

	if (state->isWall(targetX, targetY)) return false;

	// look for living actors to attack
	for (Actor* a : *actor->getActors()) {
		if (a->destructible && !a->destructible->isDead() && a->x == targetX && a->y == targetY) {
			actor->attacker->attack(actor, a);
			return true;
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

bool TraverseStairsAction::execute() {
	/* TODO something like this
	if(state->getStairs()->x == owner->x && state->getStairs()->y == owner->y) {
		state->nextLevel();
	} else {
		state->message(TCODColor::lightGrey, "There are no stairs here.");
	}
	*/
	return false;
}

bool PickupAction::execute() {
	bool found = false;
	for(Actor* a : *actor->getActors()) {
		if(a->pickable && a->x == actor->x && a->y == actor->y) {
			if(a->pickable->pick(a, actor)) {
				found = true;
				actor->s->message(TCODColor::green, "You pick up the %s.", a->name.c_str());
				return true;
			} else if(!found) {
				found = true;
				actor->s->message(TCODColor::red, "Your inventory is full.");
				return false;
			}
		}
	}
	if(!found) { actor->s->message(TCODColor::lightGrey, "There's nothing here that you can pick up."); }
	return false;
}

bool UseItemAction::execute() {
	item->pickable->use(item, actor);
	return true;
}
