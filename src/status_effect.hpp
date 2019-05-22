#ifndef STATUS_EFFECT_HPP
#define STATUS_EFFECT_HPP

#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/export.hpp>

class Actor;
class GameplayState;

#include "attribute.hpp"

class StatusEffect {
public:
	StatusEffect(std::string name = "") : name(name) {;}
	virtual ~StatusEffect() {}
	virtual void update(Actor* owner, GameplayState* state, float deltaTime) = 0;
	virtual bool isAlive() = 0;
	std::string name;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & name;
	}
};

class PoisonedStatusEffect : public StatusEffect {
public:
	PoisonedStatusEffect(int time = 10000, int interval = 1000, int damage = 2):
	StatusEffect("poisoned"), time(time), interval(interval), damage(damage) {;}

	void update(Actor* owner, GameplayState* state, float deltaTime) override;
	bool isAlive() override;
	int time;
	int interval;
	int damage;
private:
	int counter;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(StatusEffect);
		ar & time;
		ar & interval;
		ar & damage;
		ar & counter;
	}
};

class AttributeModifierStatusEffect : public StatusEffect {
public:
	AttributeModifierStatusEffect(int time = 10000, Attribute attribute = Attribute::NONE, int modifier = 0):
	StatusEffect("attribute modified"), time(time), attribute(attribute), modifier(modifier)
	{;} //FIXME name

	void update(Actor* owner, GameplayState* state, float deltaTime) override;
	bool isAlive() override;
	int time;
	Attribute attribute;
	int modifier;
	int oldValue;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(StatusEffect);
		ar & time;
		ar & attribute;
		ar & modifier;
	}
};

BOOST_CLASS_EXPORT_KEY(PoisonedStatusEffect)
BOOST_CLASS_EXPORT_KEY(AttributeModifierStatusEffect)
// remember to register new types in engine.cpp
#endif /* STATUS_EFFECT_HPP */
