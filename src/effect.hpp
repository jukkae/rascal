#ifndef EFFECT_HPP
#define EFFECT_HPP

class Actor;
class StatusEffect;
#include "ai.hpp"
#include "direction.hpp"

class Effect {
public:
	virtual bool applyTo(Actor* actor) = 0;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	}
};
//BOOST_SERIALIZATION_ASSUME_ABSTRACT(Effect)

class HealthEffect : public Effect {
public:
	float amount;

	HealthEffect(float amount = 0);
	bool applyTo(Actor* actor) override;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Effect);
		ar & amount;
	}
};

class AiChangeEffect : public Effect {
public:
	std::unique_ptr<TemporaryAi> newAi;

	AiChangeEffect(std::unique_ptr<TemporaryAi> newAi = std::unique_ptr<TemporaryAi>());
	bool applyTo(Actor* actor) override;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Effect);
		ar & newAi;
	}
};

class StatusEffectEffect : public Effect {
public:
	StatusEffectEffect(std::unique_ptr<StatusEffect> statusEffect = std::unique_ptr<StatusEffect>());
	bool applyTo(Actor* actor) override;

	std::unique_ptr<StatusEffect> statusEffect;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Effect);
		ar & statusEffect;
	}
};

class MoveEffect : public Effect {
public:
	MoveEffect(Direction direction = Direction::NONE, float distance = 1);
	bool applyTo(Actor* actor) override;

	Direction direction;
	float distance; // FIXME not implemented yet
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Effect);
		ar & direction;
		ar & distance;
	}
};

//BOOST_CLASS_EXPORT(Effect)
BOOST_CLASS_EXPORT_KEY(HealthEffect)
BOOST_CLASS_EXPORT_KEY(AiChangeEffect)
BOOST_CLASS_EXPORT_KEY(StatusEffectEffect)
BOOST_CLASS_EXPORT_KEY(MoveEffect)

#endif /* EFFECT_HPP */
