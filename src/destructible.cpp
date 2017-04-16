#include <stdio.h>
#include "main.hpp"

Destructible::Destructible(float maxHp, float defense, const char* corpseName) :
	maxHp(maxHp), hp(maxHp), defense(defense), corpseName(corpseName) {;}

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

Destructible *Destructible::create(TCODZip& zip) {
	DestructibleType type = (DestructibleType) zip.getInt();
	Destructible* destructible = NULL;
	switch(type) {
		case MONSTER: destructible = new MonsterDestructible(0,0,NULL); break;
		case PLAYER:  destructible = new PlayerDestructible (0,0,NULL); break;
	}
	destructible->load(zip);
	return destructible;
	}


MonsterDestructible::MonsterDestructible(float maxHp, float defense, const char* corpseName) :
	Destructible(maxHp, defense, corpseName) {;}

void MonsterDestructible::die(Actor* owner) {
	engine.gui->message(TCODColor::lightGrey, "%s is dead!", owner->name);
	Destructible::die(owner);
}


PlayerDestructible::PlayerDestructible(float maxHp, float defense, const char* corpseName) :
	Destructible(maxHp, defense, corpseName) {;}

void PlayerDestructible::die(Actor* owner) {
	engine.gui->message(TCODColor::red, "You died!");
	Destructible::die(owner);
	engine.gameStatus = Engine::DEFEAT;
}
