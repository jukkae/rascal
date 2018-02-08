#ifndef ATTACKER_HPP
#define ATTACKER_HPP
class Actor;
class EffectGenerator;
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

	bool attack(Actor* owner, Actor* target, int toHitBonus = 0, int toDamageBonus = 0);
	int getAttackBaseDamage();

	void increase(int delta) { bonus += delta; } // sb. checked & modified
	//std::function<int(void)> damage;
	//
	int numberOfDice;
	int dice;
	int bonus;
	std::unique_ptr<EffectGenerator> effectGenerator;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & numberOfDice;
		ar & dice;
		ar & bonus;
		ar & effectGenerator;
	}
};

// intentionally NOT subclass
class RangedAttacker {
public:
	RangedAttacker(int numberOfDice = 1, int dice = 0, int bonus = 0, float range = 0.0f, int capacity = 10, int rounds = 10):
		numberOfDice(numberOfDice), dice(dice), bonus(bonus), range(range), capacity(capacity), rounds(rounds) {;} // FIXME capacity and rounds default to something else

	bool attack(Actor* owner, Actor* target, int toHitBonus = 0, int toDamageBonus = 0);
	int getAttackBaseDamage();

	int numberOfDice;
	int dice;
	int bonus;
	float range;
	int capacity;
	int rounds;
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
