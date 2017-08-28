#include "ai.hpp"
#include "action.hpp"
#include "actor.hpp"
#include "attacker.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "gameplay_state.hpp"
#include "gui.hpp"
#include "input_handler.hpp"
#include "map.hpp"
#include "pickable.hpp"
#include "persistent.hpp"
#include <math.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include "boost/optional.hpp"

static const int TRACKING_TURNS = 3;
static const float DEFAULT_TURN_LENGTH = 100;

const int LEVEL_UP_BASE = 200;
const int LEVEL_UP_FACTOR = 150;

int PlayerAi::getNextLevelXp() const {
	return LEVEL_UP_BASE + xpLevel * LEVEL_UP_FACTOR;
}

float PlayerAi::update(Actor* owner, GameplayState* state) {
	if (owner->destructible->xp >= getNextLevelXp()) levelUpMenu(owner, state);

	// if (owner->destructible && owner->destructible->isDead()) return DEFAULT_TURN_LENGTH; TODO send DEAD event or something
	TCOD_key_t lastKey;

	boost::optional<RawInputEvent> event = state->inputHandler->getEvent();
	lastKey = event->key;

	switch(lastKey.vk) {
		case TCODK_CHAR:  handleActionKey(owner, lastKey.c, state); break;
		default: break;
	}
	return 100 * (100 / speed);
}

void PlayerAi::levelUpMenu(Actor* owner, GameplayState* state) {
	int levelUpXp = getNextLevelXp();
	++xpLevel;
	owner->destructible->xp -= levelUpXp;
	state->message(TCODColor::yellow, "You've reached level %d!", xpLevel);
	state->showLevelUpMenu();
	Menu::MenuItemCode menuItem = state->pickFromMenu(Menu::DisplayMode::PAUSE);

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

Action* PlayerAi::getNextAction(Actor* actor) {
	Direction dir;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;

	boost::optional<RawInputEvent> event = actor->s->inputHandler->getEvent();
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
			}
		}
		default: dir = Direction::NONE; break;
	}
	return new MoveAction(actor, dir); // TODO news leak memory currently
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
	for (Actor* actor : owner->container->inventory) {
		con.print(2, y, "(%c) %s", shortcut, actor->name.c_str());
		++y;
		++shortcut;
	}
	// blit the inventory console on the root console
	TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, TCODConsole::root, constants::SCREEN_WIDTH/2 - INVENTORY_WIDTH/2, constants::SCREEN_HEIGHT/2 - INVENTORY_HEIGHT/2);
	TCODConsole::flush();
	// wait for a key press
	TCOD_key_t key;

	boost::optional<RawInputEvent> event = owner->s->inputHandler->getEvent();
	key = event->key; // This probably doesn't work, but then again this whole code is not called rn

	if(key.vk == TCODK_CHAR) {
		int actorIndex=key.c - 'a';
		if(actorIndex >= 0 && actorIndex < owner->container->inventory.size()) { return owner->container->inventory.at(actorIndex); }
	}
	return nullptr;
}

void PlayerAi::handleActionKey(Actor* owner, int ascii, GameplayState* state) {
	switch(ascii) {
		case 'g' : // pickup item
		{
		bool found = false;
		for(Actor* actor : *owner->getActors()) {
			if(actor->pickable && actor->x == owner->x && actor->y == owner->y) {
				if(actor->pickable->pick(actor,owner)) {
					found = true;
					state->message(TCODColor::green, "You pick up the %s.", actor->name.c_str());
					break;
				} else if(!found) {
					found = true;
					state->message(TCODColor::red, "Your inventory is full.");
				}
			}
		}
		if(!found) { state->message(TCODColor::lightGrey, "There's nothing here that you can pick up."); }
		} break;
		case 'i' : // display inventory
		{
			Actor* actor = chooseFromInventory(owner);
			if(actor) {
				actor->pickable->use(actor, owner);
			}
		} break;
		case 'd' : // drop item
		{
			Actor* actor = chooseFromInventory(owner);
			if(actor) {
				actor->pickable->drop(actor, owner);
			}
		} break;
		case -89 : // '</>' key, damn nasty
		{
			if(state->getStairs()->x == owner->x && state->getStairs()->y == owner->y) {
				state->nextLevel();
			} else {
				state->message(TCODColor::lightGrey, "There are no stairs here.");
			}
		} break;
	}
}

float MonsterAi::update(Actor* owner, GameplayState* state) {
	if (owner->destructible && owner->destructible->isDead()) return DEFAULT_TURN_LENGTH;
	if (state->isInFov(owner->x, owner->y)) {
		moveCount = TRACKING_TURNS;
	} else { --moveCount; }
	if(moveCount > 0) {
		moveOrAttack(owner, state, state->getPlayer()->x, state->getPlayer()->y);
	}
	return 100 * (100 / speed);
}

void MonsterAi::moveOrAttack(Actor* owner, GameplayState* state, int targetX, int targetY) {
	int dx = targetX - owner->x;
	int dy = targetY - owner->y;
	int stepDx = (dx > 0 ? 1 : -1);
	int stepDy = (dy > 0 ? 1 : -1);
	float distance = sqrtf(dx*dx + dy*dy);
	if(distance >= 2) {
		state->message(TCODColor::white, "The %s threatens you!", owner->name.c_str());
		dx = (int) (round(dx / distance));
		dy = (int) (round(dy / distance));
		if(state->canWalk(owner->x + dx, owner->y + dy)) {
			owner->x += dx;
			owner->y += dy;
		} else if (state->canWalk(owner->x + stepDx, owner->y)) { // Wall sliding
			owner->x += stepDx;
		} else if (state->canWalk(owner->x, owner->y + stepDy)) {
			owner->y += stepDy;
		}
	} else if ( owner->attacker ) {
		owner->attacker->attack(owner, state->getPlayer());
	}
}

float TemporaryAi::update(Actor* owner, GameplayState* state) {
	--turns;
	if(turns <= 0) {
		owner->ai = std::move(oldAi);
	}
	return 100 * (100 / speed);
}

void TemporaryAi::applyTo(Actor* actor) {
	oldAi = std::move(actor->ai);
	actor->ai = std::unique_ptr<Ai>(this);
}

float ConfusedMonsterAi::update(Actor* owner, GameplayState* state) {
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
}

BOOST_CLASS_EXPORT(PlayerAi)
BOOST_CLASS_EXPORT(MonsterAi)
BOOST_CLASS_EXPORT(TemporaryAi)
BOOST_CLASS_EXPORT(ConfusedMonsterAi)
