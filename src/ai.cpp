#include "ai.hpp"
#include "actor.hpp"
#include "attacker.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "engine.hpp"
#include "gui.hpp"
#include "map.hpp"
#include "pickable.hpp"
#include "persistent.hpp"
#include <math.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>

static const int TRACKING_TURNS = 3;
static const float DEFAULT_TURN_LENGTH = 100;

const int LEVEL_UP_BASE = 200;
const int LEVEL_UP_FACTOR = 150;

int PlayerAi::getNextLevelXp() const {
	return LEVEL_UP_BASE + xpLevel * LEVEL_UP_FACTOR;
}

// TODO fix keyboard handling: Who handles what? currently split here, main and engine!
float PlayerAi::update(Actor* owner) {
	int levelUpXp = getNextLevelXp();
	if (owner->destructible->xp >= levelUpXp) {
		xpLevel++;
		owner->destructible->xp -= levelUpXp;
		engine.gui.message(TCODColor::yellow, "You've reached level %d!", xpLevel);
		engine.gui.menu.clear();
		engine.gui.menu.addItem(Menu::MenuItemCode::CONSTITUTION,"Constitution (+20HP)");
		engine.gui.menu.addItem(Menu::MenuItemCode::STRENGTH,"Strength (+1 attack)");
		engine.gui.menu.addItem(Menu::MenuItemCode::AGILITY,"Agility (+1 defense)");
		engine.gui.menu.addItem(Menu::MenuItemCode::SPEED,"Speed (+10 speed)");
		Menu::MenuItemCode menuItem = engine.gui.menu.pick(Menu::DisplayMode::PAUSE);

		TCODConsole::root->clear();
		engine.render();
		TCODConsole::root->flush();
		switch (menuItem) {
		case Menu::MenuItemCode::CONSTITUTION :
			owner->destructible->maxHp += 20;
			owner->destructible->hp += 20;
			break;
		case Menu::MenuItemCode::STRENGTH :
			owner->attacker->power += 1;
			break;
		case Menu::MenuItemCode::AGILITY :
			owner->destructible->defense += 1;
			break;
		case Menu::MenuItemCode::SPEED :
			owner->ai->speed += 10;
			break;
		default: break;
		}
	}
	if (owner->destructible && owner->destructible->isDead()) return DEFAULT_TURN_LENGTH;
	int dx = 0;
	int dy = 0;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse, true);
	switch(lastKey.vk) {
		case TCODK_UP:    dy = -1; break;
		case TCODK_DOWN:  dy = 1;  break;
		case TCODK_LEFT:  dx = -1; break;
		case TCODK_RIGHT: dx = 1;  break;
		case TCODK_CHAR:  handleActionKey(owner, lastKey.c); break;
		default: break;
	}
	if(dx != 0 || dy != 0) {
		engine.gameStatus = Engine::GameStatus::NEW_TURN;
		if (moveOrAttack(owner, owner->x + dx,owner->y + dy)) {
			engine.map->computeFov();
		}
	}
	if(mouse.dx != 0 || mouse.dy != 0) return 0; // TODO hack
	return 100 * (100 / speed);
}

bool PlayerAi::moveOrAttack(Actor* owner, int targetX, int targetY) {
	if (engine.map->isWall(targetX, targetY)) return false;
	// look for living actors to attack
	for (auto iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor* actor = *iterator;
		if (actor->destructible && !actor->destructible->isDead() && actor->x == targetX && actor->y == targetY) {
			owner->attacker->attack(owner, actor);
			return false;
		}
	}
	// look for corpses or items
	for (auto iterator=engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor* actor = *iterator;
		bool corpseOrItem = (actor->destructible && actor->destructible->isDead()) || actor->pickable;
		if(corpseOrItem && actor->x == targetX && actor->y == targetY) {
			engine.gui.message(TCODColor::lightGrey, "There's a %s here!", actor->name.c_str());
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
	for (auto it = owner->container->inventory.begin(); it != owner->container->inventory.end(); it++) {
		Actor* actor = *it;
		con.print(2, y, "(%c) %s", shortcut, actor->name.c_str());
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
		if(actorIndex >= 0 && actorIndex < owner->container->inventory.size()) { return owner->container->inventory.at(actorIndex); }
	}
	return nullptr;
}

void PlayerAi::handleActionKey(Actor *owner, int ascii) {
	switch(ascii) {
		case 'g' : // pickup item
		{
		bool found = false;
		for(auto i = engine.actors.begin(); i != engine.actors.end(); i++) {
			Actor* actor = *i;
			if(actor->pickable && actor->x == owner->x && actor->y == owner->y) {
				if(actor->pickable->pick(actor,owner)) {
					found = true;
					engine.gui.message(TCODColor::green, "You pick up the %s.", actor->name.c_str());
					break;
				} else if(!found) {
					found = true;
					engine.gui.message(TCODColor::red, "Your inventory is full.");
				}
			}
		}
		if(!found) { engine.gui.message(TCODColor::lightGrey, "There's nothing here that you can pick up."); }
		engine.gameStatus = Engine::GameStatus::NEW_TURN;
		} break;
		case 'i' : // display inventory
		{
			Actor* actor = chooseFromInventory(owner);
			if(actor) {
				actor->pickable->use(actor, owner);
				engine.gameStatus = Engine::GameStatus::NEW_TURN;
			}
		} break;
		case 'd' : // drop item
		{
			Actor* actor = chooseFromInventory(owner);
			if(actor) {
				actor->pickable->drop(actor, owner);
				engine.gameStatus = Engine::GameStatus::NEW_TURN;
			}
		} break;
		case -89 : // '</>' key, damn nasty
		{
			if(engine.stairs->x == owner->x && engine.stairs->y == owner->y) {
				engine.nextLevel();
			} else {
				engine.gui.message(TCODColor::lightGrey, "There are no stairs here.");
			}
		} break;
	}
}

float MonsterAi::update(Actor* owner) {
	if (owner->destructible && owner->destructible->isDead()) return DEFAULT_TURN_LENGTH;
	if (engine.map->isInFov(owner->x, owner->y)) {
		moveCount = TRACKING_TURNS;
	} else { moveCount--; }
	if(moveCount > 0) {
		moveOrAttack(owner, engine.player->x, engine.player->y);
	}
	return 100 * (100 / speed);
}

void MonsterAi::moveOrAttack(Actor* owner, int targetX, int targetY) {
	int dx = targetX - owner->x;
	int dy = targetY - owner->y;
	int stepDx = (dx > 0 ? 1 : -1);
	int stepDy = (dy > 0 ? 1 : -1);
	float distance = sqrtf(dx*dx + dy*dy);
	if(distance >= 2) {
		engine.gui.message(TCODColor::white, "The %s threatens you!", owner->name.c_str());
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

float TemporaryAi::update(Actor* owner) {
	turns--;
	if(turns <= 0) {
		owner->ai = std::move(oldAi);
	}
	return 100 * (100 / speed);
}

void TemporaryAi::applyTo(Actor* actor) {
	oldAi = std::move(actor->ai);
	actor->ai = std::unique_ptr<Ai>(this);
}

float ConfusedMonsterAi::update(Actor* owner) {
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
			Actor* target = engine.getLiveActor(destX, destY);
			if(target) { owner->attacker->attack(owner, target); }
		}
	}
	return TemporaryAi::update(owner);
}

BOOST_CLASS_EXPORT(PlayerAi)
BOOST_CLASS_EXPORT(MonsterAi)
BOOST_CLASS_EXPORT(TemporaryAi)
BOOST_CLASS_EXPORT(ConfusedMonsterAi)
