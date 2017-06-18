#include <stdio.h>
#include "main.hpp"

Destructible::Destructible(float maxHp, float defense, int xp, std::string corpseName) :
	maxHp(maxHp), hp(maxHp), defense(defense), xp(xp), corpseName(corpseName) {;}

float Destructible::takeDamage(Actor* owner, float damage) {
	damage -= defense;
	if(damage > 0) {
		hp -= damage;
		if(hp <= 0) die(owner);
	} else damage = 0;
	return damage;
}

float Destructible::heal(float amount) {
	hp += amount;
	if(hp > maxHp) {
		amount -= hp - maxHp;
		hp = maxHp;
	}
	return amount; // return the actual amount of hp restored
}

void Destructible::die(Actor *owner) {
	owner->ch = '%';
	owner->col = TCODColor::darkRed;   
	owner->name = corpseName;
	owner->blocks = false;
	// draw corpses before living actors
	engine.sendToBack(owner);
}

MonsterDestructible::MonsterDestructible(float maxHp, float defense, int xp, std::string corpseName) :
	Destructible(maxHp, defense, xp, corpseName) {;}

void MonsterDestructible::die(Actor* owner) {
	engine.gui->message(TCODColor::lightGrey, "%s is dead! You gain %d xp!", owner->name.c_str(), xp);
	engine.player->destructible->xp += xp;
	Destructible::die(owner);
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, int xp, std::string corpseName) :
	Destructible(maxHp, defense, xp, corpseName) {;}

void PlayerDestructible::die(Actor* owner) {
	engine.gui->message(TCODColor::red, "You died!");
	Destructible::die(owner);
	engine.gameStatus = Engine::GameStatus::DEFEAT;
}

template void PlayerDestructible::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void PlayerDestructible::serialize(boost::archive::text_oarchive& arch, const unsigned int version);
template void MonsterDestructible::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void MonsterDestructible::serialize(boost::archive::text_oarchive& arch, const unsigned int version);

BOOST_CLASS_EXPORT_IMPLEMENT(MonsterDestructible)
BOOST_CLASS_EXPORT_IMPLEMENT(PlayerDestructible)
