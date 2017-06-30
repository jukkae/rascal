#include <stdio.h>
#include "actor.hpp"
#include "main.hpp"

Attacker::Attacker(float power) : power(power) {;}

void Attacker::attack(Actor* owner, Actor* target) {
if(target->destructible && !target->destructible->isDead()) {
	if ( power - target->destructible->defense > 0 ) {
		engine.gui.message(TCODColor::red, "%s attacks %s for %g hit points.", owner->name.c_str(), target->name.c_str(), power - target->destructible->defense);
	} else {
		engine.gui.message(TCODColor::lightGrey, "%s attacks %s but it has no effect!", owner->name.c_str(), target->name.c_str());            
	}
	target->destructible->takeDamage(target, power);
	} else {
		engine.gui.message(TCODColor::lightGrey, "%s attacks %s in vain.", owner->name.c_str(), target->name.c_str());
	}
}
