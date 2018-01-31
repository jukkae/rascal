#include "action.hpp"
#include "actor.hpp"
#include "attacker.hpp"
#include "body.hpp"
#include "colors.hpp"
#include "comestible.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "effect.hpp"
#include "event.hpp"
#include "gameplay_state.hpp"
#include "pickable.hpp"
#include "transporter.hpp"
#include "world.hpp"

bool MoveAction::execute() {
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
				ActionFailureEvent e(actor, "You can't punch when wielding a ranged weapon!");
				world->notify(e);
				return false;
			}
			if(actor->wornWeapon && actor->wornWeapon->attacker) { 
				bool atkResult;
				if(actor->body) {
					int toHitBonus = actor->body->getModifier(actor->body->agility);
					int toDamageBonus = actor->body->getModifier(actor->body->strength);
					atkResult = actor->wornWeapon->attacker->attack(actor, a.get(), toHitBonus, toDamageBonus);
				} else {
					atkResult = actor->wornWeapon->attacker->attack(actor, a.get());
				}
				if(atkResult && actor->wornWeapon->attacker->effectGenerator) {
					//TODO handle all different effects
					std::unique_ptr<Effect> ef = actor->wornWeapon->attacker->effectGenerator->generateEffect();
					if(auto e = dynamic_cast<MoveEffect*>(ef.get())) {
						e->direction = direction;
						e->distance = 5.0f;
					}
					ef->applyTo(a.get());
				}
			}
			else {
				if(actor->body) {
					int toHitBonus = actor->body->getModifier(actor->body->agility);
					int toDamageBonus = actor->body->getModifier(actor->body->strength);
					actor->attacker->attack(actor, a.get(), toHitBonus, toDamageBonus);
				} else {
					actor->attacker->attack(actor, a.get());
				}
			}
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
	World* world = actor->world;
	GameplayState* state = world->state;
	std::vector<Actor*> v = world->getActorsAt(actor->x, actor->y);
	if(v.empty()) {
		ActionFailureEvent e(actor, "There are no stairs here!");
		world->notify(e);
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
			ActionFailureEvent e(actor, "There are no stairs down here!");
			world->notify(e);
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
			ActionFailureEvent e(actor, "There are no stairs up here!");
			world->notify(e);
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
				ActionFailureEvent e(actor, "Your inventory is full!");
				world->notify(e);
				return false;
			}
			if(a->pickable->pick(std::move(a), actor)) {
				found = true;
				ActionSuccessEvent e(actor, std::string("You pick up the ").append(itemName));
				world->notify(e);
				return true;
			}
		}
	}
	if(!found) {
		ActionFailureEvent e(actor, "There's nothing here that you can pick up!");
		world->notify(e);
	}
	return false;
}

bool UseItemAction::execute() {
	item->pickable->use(item, actor);
	return true;
}

bool DropItemAction::execute() {
	if(actor->wornWeapon == item) { // TODO order of actions is wrong
		actor->addAction(std::make_unique<UnWieldItemAction>(UnWieldItemAction(actor, item)));
	}
	for(auto& a : actor->wornArmors) {
		if(a == item) {
			actor->addAction(std::make_unique<UnWieldItemAction>(UnWieldItemAction(actor, item)));
		}
	}
	ActionSuccessEvent e(item, "You drop what you were holding!"); // TODO player-specific
	actor->world->notify(e);
	item->pickable->drop(item, actor);
	return true;
}

bool WieldItemAction::execute() {
	if(item->wieldable) {
		if(item->wieldable->wieldableType == WieldableType::ONE_HAND) {
			auto v = actor->body->getFreeBodyParts();
			if(std::find(v.begin(), v.end(), BodyPart::HAND_L) != v.end() ||
			   std::find(v.begin(), v.end(), BodyPart::HAND_R) != v.end()) {
				if(item->attacker || item->rangedAttacker) {
					for(auto& b : actor->body->bodyParts) {
						if(b.first == BodyPart::HAND_R || b.first == BodyPart::HAND_R) {
							b.second = false;
							break;
						}
					}
					actor->wornWeapon = item;
					return true;
				}
			}
		}
		if(item->wieldable->wieldableType == WieldableType::TWO_HANDS) {
			auto v = actor->body->getFreeBodyParts();
			if(std::find(v.begin(), v.end(), BodyPart::HAND_L) != v.end() &&
			   std::find(v.begin(), v.end(), BodyPart::HAND_R) != v.end()) {
				if(item->attacker || item->rangedAttacker) {
					for(auto& b : actor->body->bodyParts) {
						if(b.first == BodyPart::HAND_L || b.first == BodyPart::HAND_R) b.second = false;
					}
					actor->wornWeapon = item;
					return true;
				}
			}
		}
		if(item->wieldable->wieldableType == WieldableType::TORSO) {
			auto v = actor->body->getFreeBodyParts();
			if(std::find(v.begin(), v.end(), BodyPart::TORSO) != v.end()) {
				if(item->name == "combat armor" || item->name == "leather armor") { // TODO yeah i know
					for(auto& b : actor->body->bodyParts) {
						if(b.first == BodyPart::TORSO) b.second = false;
					}
					actor->wornArmors.push_back(item);
					return true;
				}
			}
		}
		if(item->wieldable->wieldableType == WieldableType::HEAD) {
			auto v = actor->body->getFreeBodyParts();
			if(std::find(v.begin(), v.end(), BodyPart::HEAD) != v.end()) {
				if(item->name == "combat helmet") { // TODO yeah i know
					for(auto& b : actor->body->bodyParts) {
						if(b.first == BodyPart::HEAD) b.second = false;
					}
					actor->wornArmors.push_back(item);
					return true;
				}
			}
		}
		if(item->wieldable->wieldableType == WieldableType::FEET) {
			auto v = actor->body->getFreeBodyParts();
			if(std::find(v.begin(), v.end(), BodyPart::FEET) != v.end()) {
				if(item->armor) {
					for(auto& b : actor->body->bodyParts) {
						if(b.first == BodyPart::FEET) b.second = false;
					}
					actor->wornArmors.push_back(item);
					return true;
				}
			}
		}
	}
	return false;
}

bool UnWieldItemAction::execute() {
	if(item->wieldable->wieldableType == WieldableType::TWO_HANDS) {
		for(auto& b : actor->body->bodyParts) {
			if(b.first == BodyPart::HAND_L || b.first == BodyPart::HAND_R) b.second = true;
		}
	}
	if(item->wieldable->wieldableType == WieldableType::ONE_HAND) {
		for(auto& b : actor->body->bodyParts) {
			if((b.first == BodyPart::HAND_L || b.first == BodyPart::HAND_R) && b.second == false) {
				b.second = true;
				break;
			}
		}
	}
	if(item->wieldable->wieldableType == WieldableType::TORSO) {
		for(auto& b : actor->body->bodyParts) {
			if(b.first == BodyPart::TORSO) b.second = true;
		}
	}
	if(item->wieldable->wieldableType == WieldableType::HEAD) {
		for(auto& b : actor->body->bodyParts) {
			if(b.first == BodyPart::HEAD) b.second = true;
		}
	}
	if(item->wieldable->wieldableType == WieldableType::FEET) {
		for(auto& b : actor->body->bodyParts) {
			if(b.first == BodyPart::FEET) b.second = true;
		}
	}
	if(actor->wornWeapon) {
		actor->wornWeapon = nullptr;
		return true;
	}
	if(actor->wornArmors.size() > 0) {
		auto& v = actor->wornArmors;
		if(v.end() == v.erase(std::remove_if(v.begin(), v.end(), [&](auto& a){ return a == item; }), v.end())) return true;
	}
	return false;
}

bool EatAction::execute() {
	if(item->comestible) {
		actor->body->nutrition += item->comestible->nutrition;
		ActionSuccessEvent e(item, "You eat the thing!");
		actor->world->notify(e);
		actor->container->remove(item);
		return true;
	}
	else {
		ActionFailureEvent e(item, "You can't eat that!");
		actor->world->notify(e);
		return false;
	}
}

LookAction::LookAction(Actor* actor):
	Action(actor, 50.0f), location(Point(actor->x, actor->y)) {;}

LookAction::LookAction(Actor* actor, Point location):
	Action(actor, 50.0f), location(location) {;}

bool LookAction::execute() {
	RequestDescriptionEvent e(actor, location);
	actor->world->notify(e);
	return true;
}

ShootAction::ShootAction(Actor* actor, Point target):
	Action(actor, 100.0f), target(target) {;}

bool ShootAction::execute() {
	World* world = actor->world;
	if(!actor->wornWeapon || !actor->wornWeapon->rangedAttacker) {
		ActionFailureEvent e(actor, "Shoot with what?");
		world->notify(e);
		return false;
	}

	int ax = actor->x;
	int ay = actor->y;

	int worldX = target.x;
	int worldY = target.y;

	Actor* enemy = actor->world->getLiveActor(worldX, worldY); // get some live actor at target
	if(actor->wornWeapon->rangedAttacker->rounds <= 0) {
		ActionFailureEvent e(actor, "Out of rounds!");
		world->notify(e);
		return false;
	}
	if(actor->getDistance(worldX, worldY) > actor->wornWeapon->rangedAttacker->range) {
		ActionFailureEvent e(actor, "Out of range!");
		world->notify(e);
		return false;
	}
	if( //TODO brittle and ugly, lift to function
			world->getLiveActor(ax-1, ay-1) ||
			world->getLiveActor(ax-1, ay  ) ||
			world->getLiveActor(ax-1, ay+1) ||
			world->getLiveActor(ax,   ay-1) ||
			world->getLiveActor(ax,   ay+1) ||
			world->getLiveActor(ax+1, ay-1) ||
			world->getLiveActor(ax+1, ay  ) ||
			world->getLiveActor(ax+1, ay+1)
	) {
		ActionFailureEvent e(actor, "Can't shoot when someone's grappling right next to you!");
		world->notify(e);
		return false;
	}
	else if(!enemy) {
		ActionFailureEvent e(actor, "There's nobody there!");
		world->notify(e);
		return false;
	} else {
		// TODO check for LOS
		if(actor->body) {
			int toHitBonus = actor->body->getModifier(actor->body->intelligence);
			int toDamageBonus = actor->body->getModifier(actor->body->agility);
			actor->wornWeapon->rangedAttacker->attack(actor, enemy, toHitBonus, toDamageBonus);
		} else {
			actor->wornWeapon->rangedAttacker->attack(actor, enemy);
		}
	}
	return true;
}
