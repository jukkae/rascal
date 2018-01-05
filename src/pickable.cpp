#include "pickable.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "gameplay_state.hpp"
#include "io.hpp"
#include "persistent.hpp"
#include "colors.hpp"
#include "world.hpp"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/optional.hpp>


TargetSelector::TargetSelector(SelectorType type, float range) : type(type), range(range) {;}

void TargetSelector::selectTargets(Actor* wearer, std::vector<Actor*>& list) {
	switch(type) {
		case SelectorType::CLOSEST_MONSTER :
		{
			World* world = wearer->world;
			Actor* closestMonster = world->getClosestMonster(wearer->x, wearer->y, range);
			if(closestMonster) list.push_back(closestMonster);
		} break;
		case SelectorType::SELECTED_MONSTER :
		{
			World* world = wearer->world;
			int x, y;
			UiEvent e("Left-click to select an enemy,\nor right-click to cancel.");
			world->notify(e);
			if(io::waitForMouseClick(wearer->s)) {
				x = io::mousePosition.x;
				y = io::mousePosition.y;
				Actor* actor = world->getLiveActor(x, y);
				if(actor) list.push_back(actor);
			}
		} break;
		case SelectorType::WEARER :
		{
			list.push_back(wearer);
		} break;
		case SelectorType::WEARER_RANGE :
		{
			World* world = wearer->world;
			for(auto& actor : world->getActors()) {
				if(actor.get() != wearer && actor->destructible && !actor->destructible->isDead() && actor->getDistance(wearer->x, wearer->y) <= range) {
					list.push_back(actor.get());
				}
			}
		} break;
		case SelectorType::SELECTED_RANGE :
		{
			World* world = wearer->world;
			int x, y;
			UiEvent e("Left-click to select a tile,\nor right-click to cancel.");
			world->notify(e);
			if(io::waitForMouseClick(wearer->s)) {
				x = io::mousePosition.x;
				y = io::mousePosition.y;
				for(auto& actor : world->getActors()) { // TODO highlight tiles in range
					if(actor->destructible && !actor->destructible->isDead() && actor->getDistance(x, y) <= range ) {
						list.push_back(actor.get());
					}
				}
			}
		} break;
		default: break;
	}
	if(list.size() == 0) {
		ActionFailureEvent e(wearer, "Zork! No enemy close enough!");
		wearer->world->notify(e);
	}
}

HealthEffect::HealthEffect(float amount, std::string message) : amount(amount), message(message) {;}

bool HealthEffect::applyTo(Actor* actor) {
	if(!actor->destructible) return false;
	if(amount > 0) {
		float pointsHealed = actor->destructible->heal(amount);
		if(pointsHealed > 0) {
			if(message != "") actor->s->message(colors::lightGrey, message, actor->name.c_str(), pointsHealed);
			return true;
		}
	} else {
		if(message != "" && -amount-actor->destructible->defense > 0) {
			actor->s->message(colors::lightGrey, message, actor->name.c_str(), -amount-actor->destructible->defense);
		}
		if(actor->destructible->takeDamage(actor, -amount) > 0) return true;
	}
	return false;
}

AiChangeEffect::AiChangeEffect(std::unique_ptr<TemporaryAi> newAi, std::string message) : newAi(std::move(newAi)), message(message) {;}

bool AiChangeEffect::applyTo(Actor* actor) {
	newAi->applyTo(actor);
	if(message != "") actor->s->message(colors::lightGrey, message, actor->name.c_str());
	return true;
}

StatusEffectEffect::StatusEffectEffect(std::unique_ptr<StatusEffect> statusEffect, std::string message):
statusEffect(std::move(statusEffect)), message(message) {;}

bool StatusEffectEffect::applyTo(Actor* actor) {
	actor->addStatusEffect(std::move(statusEffect));
	if(message != "") actor->s->message(colors::lightGrey, message, actor->name.c_str());
	return true;
}

MoveEffect::MoveEffect(Direction direction, float distance, std::string message):
direction(direction), distance(distance), message(message) {;}

bool MoveEffect::applyTo(Actor* actor) { // TODO finish implementation
	actor->tryToMove(direction, distance);
	if(message != "") actor->s->message(colors::lightGrey, message, actor->name.c_str());
	return true;
}

Pickable::Pickable(TargetSelector selector, std::unique_ptr<Effect> effect, int weight) : weight(weight), selector(selector), effect(std::move(effect)) {;}

// owner is the Actor that this Pickable belongs to,
// wearer is the Actor that has this Pickable in inventory.
bool Pickable::pick(std::unique_ptr<Actor> owner, Actor* wearer) {
	World* world = owner->world;
	if(wearer->container && !wearer->container->isFull()) {
		if(wearer->container->add(std::move(owner))) {
			world->getActors().erase(
				std::remove(
					world->getActors().begin(),
					world->getActors().end(),
					owner
					//[&](auto const & p) { return p.get() == owner; }
				),
				world->getActors().end()
			);
			return true;
		}
	}
	return false;
}

bool Pickable::use(Actor* owner, Actor* wearer) {
	std::vector<Actor*> list;
	if(selector.type != TargetSelector::SelectorType::NONE) {
		selector.selectTargets(wearer, list);
	}

	bool success = false;
	for(Actor* actor : list) {
		if(effect->applyTo(actor)) success = true;
	}
	if(success) {
		if(wearer->container) {
			wearer->container->remove(owner);
		}
	}
	return success;
}

void Pickable::drop(Actor* owner, Actor* wearer) {
	if(wearer->container) {
		std::string ownerName = owner->name;

		const auto it = std::find_if(
				wearer->container->inventory.begin(),
				wearer->container->inventory.end(),
				[&] (const std::unique_ptr<Actor>& a) { return owner == a.get(); });
		std::unique_ptr<Actor> item = std::move(*it); // after moving *it == nullptr -> no need to use remove_if
		wearer->container->inventory.erase(it);

		item->x = wearer->x;
		item->y = wearer->y;
		wearer->world->addActor(std::move(item));
		wearer->s->message(colors::lightGrey, "%s drops a %s.", wearer->name.c_str(), ownerName.c_str());
	}
}

// Remember to register in engine.cpp
//BOOST_CLASS_EXPORT(Effect)
BOOST_CLASS_EXPORT_IMPLEMENT(HealthEffect)
BOOST_CLASS_EXPORT_IMPLEMENT(AiChangeEffect)
BOOST_CLASS_EXPORT_IMPLEMENT(StatusEffectEffect)
BOOST_CLASS_EXPORT_IMPLEMENT(MoveEffect)
