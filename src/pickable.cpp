#include "main.hpp"

bool Pickable::pick(Actor* owner, Actor* wearer) {
	if(wearer->container && wearer->container->add(owner)) {
		engine.actors.remove(owner);
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
		engine.actors.push(owner);
		owner->x = wearer->x;
		owner->y = wearer->y;
		engine.gui->message(TCODColor::lightGrey, "%s drops a %s.", wearer->name, owner->name);
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
		engine.gui->message(TCODColor::lightGrey, "Zark! No enemy close enough to blast!");
		return false;
	}
	engine.gui->message(TCODColor::lightBlue, "A lighting bolt strikes the %s with a loud thunder!\nThe damage is %g hit points.", closestMonster->name, damage);
	closestMonster->destructible->takeDamage(closestMonster, damage);
	return Pickable::use(owner, wearer);
}

FragmentationGrenade::FragmentationGrenade (float range, float damage) : BlasterBolt(range, damage) {;}

bool FragmentationGrenade::use(Actor* owner, Actor* wearer) {
	engine.gui->message(TCODColor::cyan, "Left-click a target tile for the frag grenade,\nor right-click to cancel.");
	int x, y;
	if(!engine.pickTile(&x, &y)) { return false; }
	engine.gui->message(TCODColor::orange,"The grenade explodes, harming everything within %g tiles!", range);
	for(Actor** it = engine.actors.begin(); it != engine.actors.end(); it++) {
		Actor* actor = *it;
		if(actor->destructible && !actor->destructible->isDead() && actor->getDistance(x, y) <= range) {
			engine.gui->message(TCODColor::orange,"The %s gets hit by shrapnel for %g hit points.", actor->name,damage);
			actor->destructible->takeDamage(actor, damage);
		}
	}
	return Pickable::use(owner,wearer);
}

Confusor::Confusor(int turns, float range) : turns(turns), range(range) {;}

bool Confusor::use(Actor* owner, Actor* wearer) {
	engine.gui->message(TCODColor::cyan, "Left-click an enemy to confuse it,\nor right-click to cancel.");
	int x, y;
	if(!engine.pickTile(&x, &y)) { return false; }
	Actor* actor = engine.getActor(x, y);
	if(!actor) { return false; }
	Ai* confAi = new ConfusedMonsterAi(turns, actor->ai);
	actor->ai = confAi;
	engine.gui->message(TCODColor::lightGreen,"The eyes of the %s look empty,\nas he starts to stumble around!", actor->name);
	return Pickable::use(owner,wearer);
}
