#include "actor.hpp"
#include "ai.hpp"
#include "body.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "effect.hpp"
#include "engine.hpp"
#include "event.hpp"
#include "gameplay_state.hpp"
#include "gameover_state.hpp"
#include "colors.hpp"
#include "pickable.hpp"
#include "world.hpp"
#include <SFML/Graphics/Color.hpp>

Destructible::Destructible(float maxHp, float defense, int xp, std::string corpseName) :
	maxHp(maxHp), hp(maxHp), defense(defense), xp(xp), corpseName(corpseName) {;}

float Destructible::takeDamage(Actor* owner, float damage, DamageType type) {
	if(type == DamageType::NORMAL) damage -= defense;
	if(damage > 0) {
		if(owner->body) {
			int mod = 0;
			if(type != DamageType::RADIATION) mod = owner->body->getModifier(owner->body->agility);
			else mod = owner->body->getModifier(owner->body->endurance);
			damage -= mod;
		}
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
	owner->col = colors::get("darkerRed");
	owner->name = corpseName;
	owner->blocks = false;
	if(owner->container) {
		while(owner->container->inventory.size() > 0) {
			owner->container->inventory.at(0)->pickable->drop(owner->container->inventory.at(0).get(), owner);
		}
	}
	owner->world->getPlayer()->container->credits += 15; // TODO drop credits instead
}

MonsterDestructible::MonsterDestructible(float maxHp, float defense, int xp, std::string corpseName) :
	Destructible(maxHp, defense, xp, corpseName) {;}

void MonsterDestructible::die(Actor* owner) {
	DeathEvent e(owner, "", xp);
	owner->world->notify(e);
	PlayerAi* ai = (PlayerAi*)owner->world->getPlayer()->ai.get();
	ai->increaseXp(owner->world->getPlayer(), xp);

	Destructible::die(owner);
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, int xp, std::string corpseName) :
	Destructible(maxHp, defense, xp, corpseName) {;}

void PlayerDestructible::die(Actor* owner) {
	PlayerDeathEvent e(owner);
	owner->world->notify(e);
	Destructible::die(owner);
}

template void PlayerDestructible::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void PlayerDestructible::serialize(boost::archive::text_oarchive& arch, const unsigned int version);
template void MonsterDestructible::serialize(boost::archive::text_iarchive& arch, const unsigned int version);
template void MonsterDestructible::serialize(boost::archive::text_oarchive& arch, const unsigned int version);

BOOST_CLASS_EXPORT_IMPLEMENT(MonsterDestructible)
BOOST_CLASS_EXPORT_IMPLEMENT(PlayerDestructible)
