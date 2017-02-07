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

Actor* PlayerAi::chooseFromInventory(Actor* owner) {
	static const int INVENTORY_WIDTH = 50;
	static const int INVENTORY_HEIGHT = 28;
	static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);
	// display the inventory frame
	con.setDefaultForeground(TCODColor(200,180,50));
	con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "inventory");
	// display the items with their keyboard shortcut
	con.setDefaultForeground(TCODColor::white);
	int shortcut = 'a';
	int y = 1;
	for (Actor** it = owner->container->inventory.begin(); it != owner->container->inventory.end(); it++) {
		Actor* actor = *it;
		con.print(2, y, "(%c) %s", shortcut, actor->name);
		y++;
		shortcut++;
	}
	// blit the inventory console on the root console
	TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, TCODConsole::root, engine.screenWidth/2 - INVENTORY_WIDTH/2, engine.screenHeight/2 - INVENTORY_HEIGHT/2);
	TCODConsole::flush();
	// wait for a key press
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	if(key.vk == TCODK_CHAR) {
		int actorIndex=key.c - 'a';
		if(actorIndex >= 0 && actorIndex < owner->container->inventory.size()) { return owner->container->inventory.get(actorIndex); }
	}
	return NULL;
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
					engine.gui->message(TCODColor::green, "You pick up the %s.", actor->name);
					break;
				} else if(!found) {
					found = true;
					engine.gui->message(TCODColor::red, "Your inventory is full.");
				}
			}
		}
		if(!found) { engine.gui->message(TCODColor::lightGrey, "There's nothing here that you can pick up."); }
		engine.gameStatus = Engine::NEW_TURN;
		} break;
		case 'i' : // display inventory
		{
			Actor* actor = chooseFromInventory(owner);
			if(actor) {
				actor->pickable->use(actor, owner);
				engine.gameStatus = Engine::NEW_TURN;
			}
		} break;
		case 'd' : // drop item
		{
			Actor* actor = chooseFromInventory(owner);
			if(actor) {
				actor->pickable->drop(actor, owner);
				engine.gameStatus = Engine::NEW_TURN;
			}
		} break;
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

ConfusedMonsterAi::ConfusedMonsterAi(int turns, Ai* oldAi): turns(turns), oldAi(oldAi) {;}

void ConfusedMonsterAi::update(Actor* owner) {
	TCODRandom* rng = TCODRandom::getInstance();
	int dx = rng->getInt(-1,1);
	int dy = rng->getInt(-1,1);
	if(dx != 0 || dy != 0) {
		int destX = owner->x + dx;
		int destY = owner->y + dy;
		if(engine.map->canWalk(destX, destY)) {
			owner->x = destX;
			owner->y = destY;
		} else {
			Actor* target = engine.getActor(destX, destY);
			if(target) { owner->attacker->attack(owner, target); }
		}
	}
	turns--;
	if(turns <= 0) {
		owner->ai = oldAi;
		delete this;
	}
}
