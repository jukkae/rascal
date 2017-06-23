#include "main.hpp"

TargetSelector::TargetSelector(SelectorType type, float range) : type(type), range(range) {;}
TargetSelector::TargetSelector() : type(), range(0) {;} // TODO dirty hack

void TargetSelector::selectTargets(Actor* wearer, std::vector<Actor*>& list) {
	switch(type) {
		case SelectorType::CLOSEST_MONSTER :
		{
			Actor* closestMonster = engine.getClosestMonster(wearer->x, wearer->y, range);
			if(closestMonster) list.push_back(closestMonster);
		} break;
		case SelectorType::SELECTED_MONSTER :
		{
			int x, y;
			engine.gui.message(TCODColor::cyan, "Left-click to select an enemy,\nor right-click to cancel.");
			if(engine.pickTile(&x, &y, range)) {
				Actor* actor = engine.getLiveActor(x, y);
				if(actor) list.push_back(actor);
			}
		} break;
		case SelectorType::WEARER :
		{
			list.push_back(wearer);
		} break;
		case SelectorType::WEARER_RANGE :
		{
			for(auto it = engine.actors.begin(); it != engine.actors.end(); it++) {
				Actor* actor = *it;
				if(actor != wearer && actor->destructible && !actor->destructible->isDead() && actor->getDistance(wearer->x, wearer->y) <= range) {
					list.push_back(actor);
				}
			}
		} break;
		case SelectorType::SELECTED_RANGE :
		{
			int x, y;
			engine.gui.message(TCODColor::cyan, "Left-click to select a tile,\nor right-click to cancel.");
			if(engine.pickTile(&x, &y)) {
				for(auto it = engine.actors.begin(); it != engine.actors.end(); it++) {
					Actor* actor = *it;
					if(actor->destructible && !actor->destructible->isDead() && actor->getDistance(x, y) <= range ) {
						list.push_back(actor);
					}
				}
			}
		} break;
		default: break;
	}
	if(list.size() == 0) engine.gui.message(TCODColor::lightGrey, "Zork! No enemy close enough!");
}

HealthEffect::HealthEffect(float amount, std::string message) : amount(amount), message(message) {;}

bool HealthEffect::applyTo(Actor* actor) {
	if(!actor->destructible) return false;
	if(amount > 0) {
		float pointsHealed = actor->destructible->heal(amount);
		if(pointsHealed > 0) {
			if(message != "") engine.gui.message(TCODColor::lightGrey, message, actor->name.c_str(), pointsHealed);
			return true;
		}
	} else {
		if(message != "" && -amount-actor->destructible->defense > 0) {
			engine.gui.message(TCODColor::lightGrey, message, actor->name.c_str(), -amount-actor->destructible->defense);
		}
		if(actor->destructible->takeDamage(actor, -amount) > 0) return true;
	}
	return false;
}

AiChangeEffect::AiChangeEffect(std::unique_ptr<TemporaryAi> newAi, std::string message) : newAi(std::move(newAi)), message(message) {;}

bool AiChangeEffect::applyTo(Actor* actor) {
	newAi->applyTo(actor);
	if(message != "") engine.gui.message(TCODColor::lightGrey, message, actor->name.c_str());
	return true;
}

Pickable::Pickable(TargetSelector selector, std::unique_ptr<Effect> effect) : selector(selector), effect(std::move(effect)) {;}

// owner is the Actor that this Pickable belongs to,
// wearer is the Actor that has this Pickable in inventory.
bool Pickable::pick(Actor* owner, Actor* wearer) {
	if(wearer->container && wearer->container->add(owner)) {
		engine.actors.erase(std::remove(engine.actors.begin(), engine.actors.end(), owner), engine.actors.end());
		return true;
	}
	return false;
}

bool Pickable::use(Actor* owner, Actor* wearer) {
	std::vector<Actor*> list;
	if(selector.type != TargetSelector::NONE) {
		selector.selectTargets(wearer, list);
	}

	bool success = false;
	for(auto it = list.begin(); it != list.end(); it++) {
		if(effect->applyTo(*it)) success = true;
	}
	if(success) {
		if(wearer->container) {
			wearer->container->remove(owner);
			delete owner;
		}
	}
	return success;
}

void Pickable::drop(Actor* owner, Actor* wearer) {
	if(wearer->container) {
		wearer->container->remove(owner);
		engine.actors.push_back(owner);
		owner->x = wearer->x;
		owner->y = wearer->y;
		engine.gui.message(TCODColor::lightGrey, "%s drops a %s.", wearer->name.c_str(), owner->name.c_str());
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
