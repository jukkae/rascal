#ifndef ATTACKER_HPP
#define ATTACKER_HPP
class Actor;
#include "point.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>

class Attacker {
public :
	//Would be nice, but damn hard to serialize
	//Attacker(std::function<int(void)> damage = []() -> int { return 0; }, int power = 0);
	Attacker(int numberOfDice = 1, int dice = 0, int bonus = 0):
		numberOfDice(numberOfDice), dice(dice), bonus(bonus) {;} // default 1d0+0

	void attack(Actor* owner, Actor* target);
	int getAttackBaseDamage();

	void increase(int delta) { bonus += delta; } // sb. checked & modified
	//std::function<int(void)> damage;
	//
	int numberOfDice;
	int dice;
	int bonus;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & numberOfDice;
		ar & dice;
		ar & bonus;
	}
};

// intentionally NOT subclass
class RangedAttacker {
public:
	RangedAttacker(int numberOfDice = 1, int dice = 0, int bonus = 0, float range = 0.0f):
		numberOfDice(numberOfDice), dice(dice), bonus(bonus), range(range) {;}

	void attack(Actor* owner, Point target);

	int numberOfDice;
	int dice;
	int bonus;
	float range;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & numberOfDice;
		ar & dice;
		ar & bonus;
		ar & range;
	}
};
#endif /* ATTACKER_HPP */
