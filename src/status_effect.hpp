#ifndef STATUS_EFFECT_HPP
#define STATUS_EFFECT_HPP

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include "boost/serialization/assume_abstract.hpp"
#include <boost/serialization/base_object.hpp>

class Actor;
class GameplayState;

class StatusEffect {
public:
	virtual void update(Actor* owner, GameplayState* state, float deltaTime) = 0;
	virtual bool isAlive() = 0;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	}
};

class TestStatusEffect : public StatusEffect {
public:
	TestStatusEffect(int time = 10000) : time(time) {;}
	void update(Actor* owner, GameplayState* state, float deltaTime) override;
	bool isAlive() override;
	int time;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(StatusEffect);
		ar & time;
	}
};
BOOST_CLASS_EXPORT_KEY(TestStatusEffect)
#endif /* STATUS_EFFECT_HPP */
