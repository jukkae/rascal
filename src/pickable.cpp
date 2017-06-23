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
			// TODO
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

HealthEffect::HealthEffect(float amount, std::string message) : amount(amount), message(message) {}

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
	if(wearer->container) {
		wearer->container->remove(owner);
		delete owner;
		return true;
	}
	return false;
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

Healer::Healer(float amount) : amount(amount) {;}

bool Healer::use(Actor* owner, Actor* wearer) {
	if(wearer->destructible) {
		float amountHealed = wearer->destructible->heal(amount);
		if(amountHealed > 0) { return Pickable::use(owner, wearer); }
	}
	return false;
}

BlasterBolt::BlasterBolt(float range, float damage) : range(range), damage(damage) {;}

bool BlasterBolt::use(Actor* owner, Actor* wearer) {
	Actor* closestMonster = engine.getClosestMonster(wearer->x,wearer->y,range);
	if(!closestMonster) {
		engine.gui.message(TCODColor::lightGrey, "Zark! No enemy close enough to blast!");
		return false;
	}
	engine.gui.message(TCODColor::lightBlue, "A lighting bolt strikes the %s with a loud thunder!\nThe damage is %g hit points.", closestMonster->name.c_str(), damage);
	closestMonster->destructible->takeDamage(closestMonster, damage);
	return Pickable::use(owner, wearer);
}

FragmentationGrenade::FragmentationGrenade (float range, float damage) : BlasterBolt(range, damage) {;}

bool FragmentationGrenade::use(Actor* owner, Actor* wearer) {
	engine.gui.message(TCODColor::cyan, "Left-click a target tile for the frag grenade,\nor right-click to cancel.");
	int x, y;
	if(!engine.pickTile(&x, &y)) { return false; }
	engine.gui.message(TCODColor::orange,"The grenade explodes, harming everything within %g tiles!", range);
	for(auto it = engine.actors.begin(); it != engine.actors.end(); it++) {
		Actor* actor = *it;
		if(actor->destructible && !actor->destructible->isDead() && actor->getDistance(x, y) <= range) {
			engine.gui.message(TCODColor::orange,"The %s gets hit by shrapnel for %g hit points.", actor->name.c_str(),damage);
			actor->destructible->takeDamage(actor, damage);
		}
	}
	return Pickable::use(owner,wearer);
}

Confusor::Confusor(int turns, float range) : turns(turns), range(range) {;}

bool Confusor::use(Actor* owner, Actor* wearer) {
	engine.gui.message(TCODColor::cyan, "Left-click an enemy to confuse it,\nor right-click to cancel.");
	int x, y;
	if(!engine.pickTile(&x, &y)) { return false; }
	Actor* actor = engine.getLiveActor(x, y);
	if(!actor) { return false; }
	std::unique_ptr<Ai> confAi = std::unique_ptr<Ai>(new ConfusedMonsterAi(turns, std::move(actor->ai)));
	actor->ai = std::move(confAi);
	engine.gui.message(TCODColor::lightGreen,"The eyes of the %s look empty,\nas he starts to stumble around!", actor->name.c_str());
	return Pickable::use(owner,wearer);
}

template void Healer::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void Healer::serialize(boost::archive::text_oarchive& arch, const unsigned int version);
template void BlasterBolt::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void BlasterBolt::serialize(boost::archive::text_oarchive& arch, const unsigned int version);
template void FragmentationGrenade::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void FragmentationGrenade::serialize(boost::archive::text_oarchive& arch, const unsigned int version);
template void Confusor::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void Confusor::serialize(boost::archive::text_oarchive& arch, const unsigned int version);

BOOST_CLASS_EXPORT_IMPLEMENT(Healer)
BOOST_CLASS_EXPORT_IMPLEMENT(BlasterBolt)
BOOST_CLASS_EXPORT_IMPLEMENT(FragmentationGrenade)
BOOST_CLASS_EXPORT_IMPLEMENT(Confusor)
