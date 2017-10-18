#include <stdio.h>
#include "destructible.hpp"
#include "engine.hpp"
#include "gameplay_state.hpp"
#include "gameover_state.hpp"

Destructible::Destructible(float maxHp, float defense, int xp, std::string corpseName, int armorClass) :
	maxHp(maxHp), hp(maxHp), defense(defense), xp(xp), corpseName(corpseName), armorClass(armorClass) {;}

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

void Destructible::die(Actor* owner) {
	owner->ch = '%';
	owner->col = TCODColor::darkRed;   
	owner->name = corpseName;
	owner->blocks = false;
}

MonsterDestructible::MonsterDestructible(float maxHp, float defense, int xp, std::string corpseName, int armorClass) :
	Destructible(maxHp, defense, xp, corpseName, armorClass) {;}

void MonsterDestructible::die(Actor* owner) {
	owner->s->message(TCODColor::lightGrey, "%s is dead! You gain %d xp!", owner->name.c_str(), xp);
	PlayerAi* ai = (PlayerAi*)owner->s->getPlayer()->ai.get();
	ai->increaseXp(xp, owner->s);

	Destructible::die(owner);
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, int xp, std::string corpseName, int armorClass) :
	Destructible(maxHp, defense, xp, corpseName, armorClass) {;}

void PlayerDestructible::die(Actor* owner) {
	owner->s->message(TCODColor::red, "You died!");
	Destructible::die(owner);
	Engine* engine = owner->s->getEngine();
	State* gameOverState = new GameOverState(owner);
	gameOverState->init(engine);
	engine->pushState(gameOverState);
}

template void PlayerDestructible::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void PlayerDestructible::serialize(boost::archive::text_oarchive& arch, const unsigned int version);
template void MonsterDestructible::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void MonsterDestructible::serialize(boost::archive::text_oarchive& arch, const unsigned int version);

BOOST_CLASS_EXPORT_IMPLEMENT(MonsterDestructible)
BOOST_CLASS_EXPORT_IMPLEMENT(PlayerDestructible)
