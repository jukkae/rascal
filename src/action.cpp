#include "action.hpp"
#include "actor.hpp"
#include "attacker.hpp"
#include "colors.hpp"
#include "gameplay_state.hpp"

bool MoveAction::execute() {
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
			state->message(colors::lightGrey, "There's a %s here!", a->name.c_str());
		}
	}
	actor->x = targetX;
	actor->y = targetY;
	return true;
}

bool TraverseStairsAction::execute() {
	GameplayState* state = actor->s;
	if(down) {
		if(state->getStairs()->x == actor->x && state->getStairs()->y == actor->y) {
			state->nextLevel();
			return true;
		} else {
			state->message(colors::lightGrey, "There are no stairs here.");
			return false;
		}
	}

	else {
		state->message(colors::lightGrey, "There are no stairs leading up here.");
		return false; // up not implemented yet
	}
}

bool PickupAction::execute() {
	bool found = false;
	for(Actor* a : *actor->getActors()) {
		if(a->pickable && a->x == actor->x && a->y == actor->y) {
			if(a->pickable->pick(a, actor)) {
				found = true;
				actor->s->message(colors::green, "You pick up the %s.", a->name.c_str());
				return true;
			} else if(!found) {
				found = true;
				actor->s->message(colors::red, "Your inventory is full.");
				return false;
			}
		}
	}
	if(!found) { actor->s->message(colors::lightGrey, "There's nothing here that you can pick up."); }
	return false;
}

bool UseItemAction::execute() {
	item->pickable->use(item, actor);
	return true;
}
