#ifndef PICKABLE_HPP
#define PICKABLE_HPP

#include "actor.hpp"
#include "ai.hpp"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include "boost/serialization/assume_abstract.hpp"
#include <boost/serialization/base_object.hpp>

#include "persistent.hpp"

class TargetSelector {
public:
	enum class SelectorType { CLOSEST_MONSTER, SELECTED_MONSTER, WEARER, WEARER_RANGE, SELECTED_RANGE, NONE };
	TargetSelector(SelectorType type = SelectorType::NONE, float range = 0);
	void selectTargets(Actor* wearer, std::vector<Actor*>& list);
	SelectorType type;
protected:
	float range;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & type;
		ar & range;
	}
};

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
	std::string message;

	HealthEffect(float amount = 0, std::string message = "");
	bool applyTo(Actor* actor) override;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Effect);
		ar & amount;
		ar & message;
	}
};

class AiChangeEffect : public Effect {
public:
	std::unique_ptr<TemporaryAi> newAi;
	std::string message;

	AiChangeEffect(std::unique_ptr<TemporaryAi> newAi = std::unique_ptr<TemporaryAi>(), std::string message = "");
	bool applyTo(Actor* actor) override;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Effect);
		ar & newAi;
		ar & message;
	}
};

class Pickable {
public:
	Pickable(TargetSelector selector = TargetSelector(TargetSelector::SelectorType::NONE, 0), std::unique_ptr<Effect> effect = std::unique_ptr<Effect>());
	virtual ~Pickable() {};
	bool pick(std::unique_ptr<Actor> owner, Actor* wearer);
	bool use (Actor* owner, Actor* wearer);
	void drop(Actor* owner, Actor* wearer);
protected:
	TargetSelector selector;
	std::unique_ptr<Effect> effect;
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
		ar & selector;
		ar & effect;
    }
};

//BOOST_CLASS_EXPORT(Effect)
BOOST_CLASS_EXPORT_KEY(HealthEffect)
BOOST_CLASS_EXPORT_KEY(AiChangeEffect)	

#endif /* PICKABLE_HPP */
