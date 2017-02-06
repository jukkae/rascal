#include "main.hpp"
#include <math.h>

void PlayerAi::update(Actor* owner) {
	if (owner->destructible && owner->destructible->isDead()) return;
	int dx = 0;
	int dy = 0;
	switch(engine.lastKey.vk) {
		case TCODK_UP:    dy = -1; break;
		case TCODK_DOWN:  dy = 1;  break;
		case TCODK_LEFT:  dx = -1; break;
		case TCODK_RIGHT: dx = 1;  break;
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
	// look for corpses
	for (Actor **iterator=engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor *actor=*iterator;
		if ( actor->destructible && actor->destructible->isDead() && actor->x == targetX && actor->y == targetY) {
			printf ("There's a %s here\n",actor->name);
		}
	}
	owner->x = targetX;
	owner->y = targetY;
	return true;
}

void MonsterAi::update(Actor* owner) {
	if (owner->destructible && owner->destructible->isDead()) return;
	if (engine.map->isInFov(owner->x, owner->y)) {
		printf("The %s growls!\n", owner->name);
		moveOrAttack(owner, engine.player->x, engine.player->y);
	}
}

void MonsterAi::moveOrAttack(Actor* owner, int targetX, int targetY) {
	int dx = targetX - owner->x;
	int dy = targetY - owner->y;
	float distance = sqrtf(dx*dx + dy*dy);
	if(distance >= 2) {
		dx = (int) (round(dx / distance));
		dy = (int) (round(dy / distance));
		if(engine.map->canWalk(owner->x + dx, owner->y + dy)) {
			owner->x += dx;
			owner->y += dy;
		}
	} else if ( owner->attacker ) {
		owner->attacker->attack(owner, engine.player);
	}
}

