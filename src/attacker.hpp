#ifndef ATTACKER_HPP
#define ATTACKER_HPP
class Actor;
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>

class Attacker {
public :
	float power;

	Attacker(float power = 0);
	void attack(Actor* owner, Actor* target);

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & power;
	}
};
#endif /* ATTACKER_HPP */
