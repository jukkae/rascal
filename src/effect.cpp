#include "effect.hpp"

#include "actor.hpp"
#include "ai.hpp"
#include "destructible.hpp"
#include "event.hpp"
#include "world.hpp"

HealthEffect::HealthEffect(float amount) : amount(amount) {;}

bool HealthEffect::applyTo(Actor* actor) {
	if(!actor->destructible) return false;
	if(amount > 0) {
		float pointsHealed = actor->destructible->heal(amount);
		if(pointsHealed > 0) {
			PickableHealthEffectEvent e(actor, pointsHealed);
			actor->world->notify(e);
			return true;
		}
	} else {
		PickableHealthEffectEvent e(actor, amount-actor->destructible->defense);
		actor->world->notify(e);
		if(actor->destructible->takeDamage(actor, -amount) > 0) return true;
	}
	return false;
}

AiChangeEffect::AiChangeEffect(std::unique_ptr<TemporaryAi> newAi) : newAi(std::move(newAi)) {;}

bool AiChangeEffect::applyTo(Actor* actor) {
	newAi->applyTo(actor);
	AiChangeEvent e(actor);
	actor->world->notify(e);
	return true;
}

StatusEffectEffect::StatusEffectEffect(std::unique_ptr<StatusEffect> statusEffect):
statusEffect(std::move(statusEffect)) {;}

bool StatusEffectEffect::applyTo(Actor* actor) {
	StatusEffectChangeEvent e(actor, statusEffect.get());
	actor->world->notify(e);
	actor->addStatusEffect(std::move(statusEffect));
	return true;
}

StatusEffectRemovalEffect::StatusEffectRemovalEffect(std::unique_ptr<StatusEffect> statusEffect):
statusEffect(std::move(statusEffect)) {;}

bool StatusEffectRemovalEffect::applyTo(Actor* actor) {
	StatusEffectChangeEvent e(actor, statusEffect.get());
	actor->world->notify(e);
	actor->removeStatusEffect(); //TODO wrong event, no params
	return true;
}

MoveEffect::MoveEffect(Direction direction, float distance):
direction(direction), distance(distance) {;}

bool MoveEffect::applyTo(Actor* actor) {
	GenericActorEvent e(actor, "The %s is pushed back!");
	actor->world->notify(e);
	actor->tryToMove(direction, distance);
	return true;
}

// Remember to register in engine.cpp
//BOOST_CLASS_EXPORT(Effect)
BOOST_CLASS_EXPORT_IMPLEMENT(HealthEffect)
BOOST_CLASS_EXPORT_IMPLEMENT(AiChangeEffect)
BOOST_CLASS_EXPORT_IMPLEMENT(StatusEffectEffect)
BOOST_CLASS_EXPORT_IMPLEMENT(StatusEffectRemovalEffect)
BOOST_CLASS_EXPORT_IMPLEMENT(MoveEffect)
