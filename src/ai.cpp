#include "main.hpp"
#include <math.h>

static const int TRACKING_TURNS = 3;

void PlayerAi::update(Actor* owner) {
	if (owner->destructible && owner->destructible->isDead()) return;
	int dx = 0;
	int dy = 0;
	switch(engine.lastKey.vk) {
		case TCODK_UP:    dy = -1; break;
		case TCODK_DOWN:  dy = 1;  break;
		case TCODK_LEFT:  dx = -1; break;
		case TCODK_RIGHT: dx = 1;  break;
		case TCODK_CHAR:  handleActionKey(owner, engine.lastKey.c); break;
		default: break;
	}
	if(dx != 0 || dy != 0) {
		engine.gameStatus = Engine::NEW_TURN;
		if (moveOrAttack(owner, owner->x + dx,owner->y + dy)) {
			engine.map->computeFov();
		}
	}
}

bool PlayerAi::moveOrAttack(Actor* owner, int targetX, int targetY) {
	if (engine.map->isWall(targetX, targetY)) return false;
	// look for living actors to attack
	for (Actor** iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor* actor = *iterator;
		if (actor->destructible && !actor->destructible->isDead() && actor->x == targetX && actor->y == targetY) {
			owner->attacker->attack(owner, actor);
			return false;
		}
	}
	// look for corpses or items
	for (Actor** iterator=engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor* actor = *iterator;
		bool corpseOrItem = (actor->destructible && actor->destructible->isDead()) || actor->pickable;
		if(corpseOrItem && actor->x == targetX && actor->y == targetY) {
			engine.gui->message(TCODColor::lightGrey, "There's a %s here!", actor->name);
		}
	}
	owner->x = targetX;
	owner->y = targetY;
	return true;
}

void PlayerAi::handleActionKey(Actor *owner, int ascii) {
	switch(ascii) {
		case 'g' : // pickup item
			{
			bool found = false;
			for(Actor** i = engine.actors.begin(); i != engine.actors.end(); i++) {
				Actor* actor = *i;
				if(actor->pickable && actor->x == owner->x && actor->y == owner->y) {
					if(actor->pickable->pick(actor,owner)) {
						found = true;
						engine.gui->message(TCODColor::lightGrey, "You pick up the %s.", actor->name);
						break;
					} else if(!found) {
						found = true;
						engine.gui->message(TCODColor::red, "Your inventory is full.");
					}
				}
			}
			if(!found) { engine.gui->message(TCODColor::lightGrey, "There's nothing here that you can pick up."); }
			engine.gameStatus = Engine::NEW_TURN;
			}
			break;
	}
}

void MonsterAi::update(Actor* owner) {
	if (owner->destructible && owner->destructible->isDead()) return;
	if (engine.map->isInFov(owner->x, owner->y)) {
		moveCount = TRACKING_TURNS;
	} else { moveCount--; }
	if(moveCount > 0) {
		engine.gui->message(TCODColor::white, "The %s threatens you!", owner->name);
		moveOrAttack(owner, engine.player->x, engine.player->y);
	}
}

void MonsterAi::moveOrAttack(Actor* owner, int targetX, int targetY) {
	int dx = targetX - owner->x;
	int dy = targetY - owner->y;
	int stepDx = (dx > 0 ? 1 : -1);
	int stepDy = (dy > 0 ? 1 : -1);
	float distance = sqrtf(dx*dx + dy*dy);
	if(distance >= 2) {
		dx = (int) (round(dx / distance));
		dy = (int) (round(dy / distance));
		if(engine.map->canWalk(owner->x + dx, owner->y + dy)) {
			owner->x += dx;
			owner->y += dy;
		} else if (engine.map->canWalk(owner->x + stepDx, owner->y)) { // Wall sliding
			owner->x += stepDx;
		} else if (engine.map->canWalk(owner->x, owner->y + stepDy)) {
			owner->y += stepDy;
		}
	} else if ( owner->attacker ) {
		owner->attacker->attack(owner, engine.player);
	}
}

