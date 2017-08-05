#include "pickable.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "engine.hpp"
#include "gameplay_state.hpp"
#include "persistent.hpp"

TargetSelector::TargetSelector(SelectorType type, float range) : type(type), range(range) {;}

void TargetSelector::selectTargets(Actor* wearer, std::vector<Actor*>& list) {
	switch(type) {
		case SelectorType::CLOSEST_MONSTER :
		{
			Actor* closestMonster = wearer->getClosestMonster(wearer->x, wearer->y, range);
			if(closestMonster) list.push_back(closestMonster);
		} break;
		case SelectorType::SELECTED_MONSTER :
		{
			int x, y;
			wearer->s->message(TCODColor::cyan, "Left-click to select an enemy,\nor right-click to cancel.");
			if(engine.pickTile(&x, &y, range)) {
				Actor* actor = wearer->getLiveActor(x, y);
				if(actor) list.push_back(actor);
			}
		} break;
		case SelectorType::WEARER :
		{
			list.push_back(wearer);
		} break;
		case SelectorType::WEARER_RANGE :
		{
			for(Actor* actor : *wearer->getActors()) {
				if(actor != wearer && actor->destructible && !actor->destructible->isDead() && actor->getDistance(wearer->x, wearer->y) <= range) {
					list.push_back(actor);
				}
			}
		} break;
		case SelectorType::SELECTED_RANGE :
		{
			int x, y;
			wearer->s->message(TCODColor::cyan, "Left-click to select a tile,\nor right-click to cancel.");
			if(engine.pickTile(&x, &y)) {
				for(Actor* actor : *wearer->getActors()) {
					if(actor->destructible && !actor->destructible->isDead() && actor->getDistance(x, y) <= range ) {
						list.push_back(actor);
					}
				}
			}
		} break;
		default: break;
	}
	if(list.size() == 0) wearer->s->message(TCODColor::lightGrey, "Zork! No enemy close enough!");
}

HealthEffect::HealthEffect(float amount, std::string message) : amount(amount), message(message) {;}

bool HealthEffect::applyTo(Actor* actor) {
	if(!actor->destructible) return false;
	if(amount > 0) {
		float pointsHealed = actor->destructible->heal(amount);
		if(pointsHealed > 0) {
			if(message != "") actor->s->message(TCODColor::lightGrey, message, actor->name.c_str(), pointsHealed);
			return true;
		}
	} else {
		if(message != "" && -amount-actor->destructible->defense > 0) {
			actor->s->message(TCODColor::lightGrey, message, actor->name.c_str(), -amount-actor->destructible->defense);
		}
		if(actor->destructible->takeDamage(actor, -amount) > 0) return true;
	}
	return false;
}

AiChangeEffect::AiChangeEffect(std::unique_ptr<TemporaryAi> newAi, std::string message) : newAi(std::move(newAi)), message(message) {;}

bool AiChangeEffect::applyTo(Actor* actor) {
	newAi->applyTo(actor);
	if(message != "") actor->s->message(TCODColor::lightGrey, message, actor->name.c_str());
	return true;
}

Pickable::Pickable(TargetSelector selector, std::unique_ptr<Effect> effect) : selector(selector), effect(std::move(effect)) {;}

// owner is the Actor that this Pickable belongs to,
// wearer is the Actor that has this Pickable in inventory.
bool Pickable::pick(Actor* owner, Actor* wearer) {
	if(wearer->container && wearer->container->add(owner)) {
		wearer->getActors()->erase(std::remove(wearer->getActors()->begin(), wearer->getActors()->end(), owner), wearer->getActors()->end());
		return true;
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
			owner = 0; // let's make sure we don't delete an already-deleted pointer
			delete owner;
		}
	}
	return success;
}

void Pickable::drop(Actor* owner, Actor* wearer) {
	if(wearer->container) {
		wearer->container->remove(owner);
		wearer->getActors()->push_back(owner);
		owner->x = wearer->x;
		owner->y = wearer->y;
		wearer->s->message(TCODColor::lightGrey, "%s drops a %s.", wearer->name.c_str(), owner->name.c_str());
	}
}

template void Effect::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void Effect::serialize(boost::archive::text_oarchive& arch, const unsigned int version);
template void HealthEffect::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void HealthEffect::serialize(boost::archive::text_oarchive& arch, const unsigned int version);
template void AiChangeEffect::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void AiChangeEffect::serialize(boost::archive::text_oarchive& arch, const unsigned int version);
template void Pickable::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void Pickable::serialize(boost::archive::text_oarchive& arch, const unsigned int version);

BOOST_CLASS_EXPORT_IMPLEMENT(Effect)
BOOST_CLASS_EXPORT_IMPLEMENT(HealthEffect)
BOOST_CLASS_EXPORT_IMPLEMENT(AiChangeEffect)
