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
	void update(Actor* owner, GameplayState* state, float deltaTime);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	}

};

#endif /* STATUS_EFFECT_HPP */
