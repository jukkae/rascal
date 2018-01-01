#include <cstdio>
#include "attacker.hpp"
#include "actor.hpp"
#include "dice.hpp"
#include "destructible.hpp"
#include "gameplay_state.hpp"
#include "colors.hpp"

void Attacker::attack(Actor* owner, Actor* target) {
	if(target->destructible && !target->destructible->isDead()) {
		int attackRoll = d20();
		if(attackRoll > target->destructible->armorClass) {
			int dmg = getAttackBaseDamage();
			if ( dmg - target->destructible->defense > 0 ) {
				if(owner->wornWeapon) owner->s->message(colors::red, "%s attacks %s for %g hit points with a %s.", owner->name.c_str(), target->name.c_str(), dmg - target->destructible->defense, owner->wornWeapon->name.c_str());
				else owner->s->message(colors::red, "%s attacks %s for %g hit points.", owner->name.c_str(), target->name.c_str(), dmg - target->destructible->defense);
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
	int dmg = 0;
	for(int i = 0; i < numberOfDice; i++) {
		int d = 0;
		switch (dice) {
			case 0:  d = 0; break;
			case 1:  d = 1; break;
			case 2:  d = d2(); break;
			case 3:  d = d3(); break;
			case 4:  d = d4(); break;
			case 5:  d = d5(); break;
			case 6:  d = d6(); break;
			case 8:  d = d8(); break;
			case 10: d = d10(); break;
			case 12: d = d12(); break;
			case 20: d = d20(); break;
			default: break;
		}
		dmg += d;
	}
	dmg += bonus;
	return dmg;
}

void RangedAttacker::attack(Actor* owner, Actor* target) {
	std::cout << "rounds: " << rounds << "\n";
	if(rounds < 1) {
		owner->s->message(colors::red, "Out of rounds!");
		return;
	}
	--rounds;
	if(target->destructible && !target->destructible->isDead()) {
		int attackRoll = d20();
		if(attackRoll > target->destructible->armorClass) {
			int dmg = getAttackBaseDamage();
			if ( dmg - target->destructible->defense > 0 ) {
				if(owner->wornWeapon) owner->s->message(colors::red, "%s shoots %s for %g hit points with a %s.", owner->name.c_str(), target->name.c_str(), dmg - target->destructible->defense, owner->wornWeapon->name.c_str());
				else owner->s->message(colors::red, "%s shoots %s for %g hit points.", owner->name.c_str(), target->name.c_str(), dmg - target->destructible->defense);
			} else {
				owner->s->message(colors::lightGrey, "%s shoots %s but it has no effect!", owner->name.c_str(), target->name.c_str());
			}
			target->destructible->takeDamage(target, dmg);
		} else {
			owner->s->message(colors::lightGrey, "%s misses %s!", owner->name.c_str(), target->name.c_str());
		}
	}
	else {
		owner->s->message(colors::lightGrey, "%s shoots %s in vain.", owner->name.c_str(), target->name.c_str());
	}
}

int RangedAttacker::getAttackBaseDamage() {
	int dmg = 0;
	for(int i = 0; i < numberOfDice; i++) {
		int d = 0;
		switch (dice) {
			case 0:  d = 0; break;
			case 1:  d = 1; break;
			case 2:  d = d2(); break;
			case 3:  d = d3(); break;
			case 4:  d = d4(); break;
			case 5:  d = d5(); break;
			case 6:  d = d6(); break;
			case 8:  d = d8(); break;
			case 10: d = d10(); break;
			case 12: d = d12(); break;
			case 20: d = d20(); break;
			default: break;
		}
		dmg += d;
	}
	dmg += bonus;
	return dmg;
}
