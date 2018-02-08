#include <cstdio>
#include "attacker.hpp"
#include "actor.hpp"
#include "body.hpp"
#include "dice.hpp"
#include "destructible.hpp"
#include "effect.hpp"
#include "event.hpp"
#include "gameplay_state.hpp"
#include "colors.hpp"
#include "world.hpp"

bool Attacker::attack(Actor* owner, Actor* target, int toHitBonus, int toDamageBonus) {
	if(target->destructible && !target->destructible->isDead()) {
		int attackRoll = d20() + toHitBonus;
		if(attackRoll > target->getAC()) {
			int dmg = getAttackBaseDamage() + toDamageBonus;
			int damage = dmg - target->destructible->defense;
			MeleeHitEvent e(owner, target, owner->wornWeapon, damage, true);
			owner->world->notify(e);

			target->destructible->takeDamage(target, dmg);

			if(attackRoll + owner->body->getModifier(owner->body->luck) >= 20) {
				UiEvent ue("It's a critical hit!");
				owner->world->notify(ue);
				MeleeHitEvent e(owner, target, owner->wornWeapon, damage/2, true);
				owner->world->notify(e);

				target->destructible->takeDamage(target, dmg/2);
			}
			return true;
		} else {
			MeleeHitEvent e(owner, target, nullptr, 0, false);
			owner->world->notify(e);
			return false;
		}
	}
	else {
		return false;
		//TODO hitting a dead corpse
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

bool RangedAttacker::attack(Actor* owner, Actor* target, int toHitBonus, int toDamageBonus) {
	RangedHitEvent e(owner, target);
	--rounds;
	if(target->destructible && !target->destructible->isDead()) {
		int attackRoll = d20() + toHitBonus;
		if(attackRoll > target->getAC()) {
			e.hit = true;
			int dmg = getAttackBaseDamage() + toDamageBonus;
			if ( dmg - target->destructible->defense > 0 ) {
				if(owner->wornWeapon) {
					e.damage = dmg - target->destructible->defense;
					e.weapon = owner->wornWeapon;
				}
			} else { // TODO what happens here
				e.damage = dmg - target->destructible->defense;
				e.weapon = owner->wornWeapon;
			}
			owner->world->notify(e);
			target->destructible->takeDamage(target, dmg);
			return true;
		}
		return false;
	}
	else {
		owner->world->notify(e);
		//TODO shooting at dead bodies
		return true;
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
