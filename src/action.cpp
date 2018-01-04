#include "action.hpp"
#include "actor.hpp"
#include "attacker.hpp"
#include "colors.hpp"
#include "event.hpp"
#include "gameplay_state.hpp"
#include "world.hpp"

bool MoveAction::execute() {
	GameplayState* state = actor->s;
	World* world = actor->world;
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

	if (world->isWall(targetX, targetY)) return false;

	// look for living actors to attack
	for (auto& a : world->getActors()) {
		if (a->destructible && !a->destructible->isDead() && a->x == targetX && a->y == targetY) {
			if(actor->wornWeapon && actor->wornWeapon->rangedAttacker) {
				state->message(colors::red, "You can't shoot from this distance!");
				return false;
			}
			if(actor->wornWeapon && actor->wornWeapon->attacker) actor->wornWeapon->attacker->attack(actor, a.get());
			else actor->attacker->attack(actor, a.get());
			return true;
		}
	}
	// look for corpses or items
	for (auto& a : world->getActors()) {
		bool corpseOrItem = (a->destructible && a->destructible->isDead()) || a->pickable;
		if(corpseOrItem && a->x == targetX && a->y == targetY) {
			ItemFoundEvent e(actor, a.get());
			world->notify(e);
		}
	}
	actor->x = targetX;
	actor->y = targetY;
	return true;
}

bool TraverseStairsAction::execute() {
	GameplayState* state = actor->s;
	World* world = actor->world;
	std::vector<Actor*> v = world->getActorsAt(actor->x, actor->y);
	if(v.empty()) {
		state->message(colors::lightGrey, "There are no stairs here.");
		return false;
	}

	if(down) {
		Actor* stairsDown = nullptr;
		auto it = std::find_if(v.begin(), v.end(), [](const auto& a) {
				  return a->transporter ? a->transporter->direction == VerticalDirection::DOWN : false;
				  });
		if(it != v.end()) stairsDown = *it;

		if(stairsDown) {
			state->previousLevel();
			return true;
		} else {
			state->message(colors::lightGrey, "There are no stairs down here.");
			return false;
		}
	} else {
		Actor* stairsUp = nullptr;
		auto it = std::find_if(v.begin(), v.end(), [](const auto& a) {
				  return a->transporter ? a->transporter->direction == VerticalDirection::UP : false;
				  });
		if(it != v.end()) stairsUp = *it;

		if(stairsUp) {
			state->nextLevel();
			return true;
		} else {
			state->message(colors::lightGrey, "There are no stairs up here.");
			return false;
		}
	}
}

bool PickupAction::execute() {
	bool found = false;
	World* world = actor->world;
	for(auto& a : world->getActors()) {
		if(a->pickable && a->x == actor->x && a->y == actor->y) {
			std::string itemName = a->name;
			if(actor->container->isFull()) {
				found = true;
				actor->s->message(colors::red, "Your inventory is full.");
				return false;
			}
			if(a->pickable->pick(std::move(a), actor)) {
				found = true;
				actor->s->message(colors::green, "You pick up the %s.", itemName.c_str());
				return true;
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

bool DropItemAction::execute() {
	if(actor->wornWeapon == item) { // TODO order of actions is wrong
		actor->addAction(std::make_unique<UnWieldItemAction>(UnWieldItemAction(actor)));
	}
	item->pickable->drop(item, actor);
	return true;
}

bool WieldItemAction::execute() {
	if(item->attacker || item->rangedAttacker) {
		actor->wornWeapon = item;
		return true;
	} else return false;
}

bool UnWieldItemAction::execute() {
	if(actor->wornWeapon) {
		actor->wornWeapon = nullptr;
		return true;
	} else return false;
}

LookAction::LookAction(Actor* actor):
	Action(actor, 50.0f), location(Point(actor->x, actor->y)) {;}

LookAction::LookAction(Actor* actor, Point location):
	Action(actor, 50.0f), location(location) {;}

bool LookAction::execute() {
	actor->s->message(colors::lightGrey, "You take a look around. The building is as bleak from inside.");
	return true;
}

ShootAction::ShootAction(Actor* actor, Point target):
	Action(actor, 100.0f), target(target) {;}

bool ShootAction::execute() {
	if(!actor->wornWeapon || !actor->wornWeapon->rangedAttacker) {
		actor->s->message(colors::lightRed, "Shoot with what?");
		return false;
	}

	int worldX = target.x;
	int worldY = target.y;

	Actor* enemy = actor->world->getLiveActor(worldX, worldY); // get some live actor at target
	if(actor->wornWeapon->rangedAttacker->rounds <= 0) {
		actor->s->message(colors::red, "Out of rounds!");
		return false;
	}
	if(actor->getDistance(worldX, worldY) > actor->wornWeapon->rangedAttacker->range) {
		actor->s->message(colors::lightRed, "You can't shoot that far");
		return false;
	}
	else if(!enemy) {
		actor->s->message(colors::lightRed, "There's nobody there!");
		return false;
	} else {
		// TODO check for LOS
		actor->wornWeapon->rangedAttacker->attack(actor, enemy);
	}
	return true;
}
