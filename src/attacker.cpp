#include <stdio.h>
#include "attacker.hpp"
#include "actor.hpp"
#include "dice.hpp"
#include "destructible.hpp"
#include "gameplay_state.hpp"
#include "colors.hpp"

Attacker::Attacker(int power) : power(power) {;}

void Attacker::attack(Actor* owner, Actor* target) {
	if(target->destructible && !target->destructible->isDead()) {
		int attackRoll = d20();
		if(attackRoll > target->destructible->armorClass) {
			int dmg = getAttackBaseDamage();
			if ( dmg - target->destructible->defense > 0 ) {
				owner->s->message(colors::red, "%s attacks %s for %g hit points.", owner->name.c_str(), target->name.c_str(), dmg - target->destructible->defense);
			} else {
				owner->s->message(colors::lightGrey, "%s attacks %s but it has no effect!", owner->name.c_str(), target->name.c_str());
			}
			target->destructible->takeDamage(target, dmg);
		} else {
			owner->s->message(colors::lightGrey, "%s misses %s!", owner->name.c_str(), target->name.c_str());
		}
	}
	else {
		owner->s->message(colors::lightGrey, "%s attacks %s in vain.", owner->name.c_str(), target->name.c_str());
	}
}

int Attacker::getAttackBaseDamage() {
	// TODO quick test!
	switch (power) {
		case 5: // player
			return d6();
		case 4: // fighter
			return d8();
		case 3: // punk
			return d4();
		default:
			return d6();
	}
}
