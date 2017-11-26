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
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	}
};

class PermanentStatusEffect : public StatusEffect {
public:
	virtual void update(Actor* owner, GameplayState* state, float deltaTime) override = 0;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	}
};

class TemporaryStatusEffect : public StatusEffect {
public:
	TemporaryStatusEffect(int time = 10000) : time(time) {;} // Default time currently for ser
	virtual void update(Actor* owner, GameplayState* state, float deltaTime) override = 0;
	int time;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(StatusEffect);
		ar & time;
	}
}; // TODO does not deserialize

class TestStatusEffect : public TemporaryStatusEffect {
public:
	TestStatusEffect(int time = 10000) : TemporaryStatusEffect(time) {;}
	virtual void update(Actor* owner, GameplayState* state, float deltaTime) override;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(TemporaryStatusEffect);
	}
};
BOOST_CLASS_EXPORT_KEY(TestStatusEffect)
BOOST_CLASS_EXPORT_KEY(TemporaryStatusEffect)
#endif /* STATUS_EFFECT_HPP */
