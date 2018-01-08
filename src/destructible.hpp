#ifndef DESTRUCTIBLE_HPP
#define DESTRUCTIBLE_HPP

#include "actor.hpp" // must be included for serialization
#include "damage.hpp"

class Destructible {
public:
	float maxHp;
	float hp;
	float defense;
	int xp; // PlayerDestructible should maybe not have xp?
	std::string corpseName;
	int armorClass;
	
	Destructible(float maxHp = 0, float defense = 0, int xp = 0, std::string corpseName = "corpse", int armorClass = 10);
	virtual ~Destructible() {};

	inline bool isDead() { return hp <= 0; }
	float takeDamage(Actor* owner, float damage, DamageType type = DamageType::NORMAL);
	float heal(float amount);
	virtual void die(Actor* owner);

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & maxHp;
		ar & hp;
		ar & defense;
		ar & xp;
		ar & corpseName;
		ar & armorClass;
	}
};

class MonsterDestructible : public Destructible {
public :
	MonsterDestructible(float maxHp = 0, float defense = 0, int xp = 0, std::string corpseName = "corpse", int armorClass = 10);
	void die(Actor* owner);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar.template register_type<Destructible>();
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Destructible);
	}
};

class PlayerDestructible : public Destructible {
public :
	PlayerDestructible(float maxHp = 0, float defense = 0, int xp = 0, std::string corpseName = "your corpse", int armorClass = 10);
	void die(Actor* owner);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar.template register_type<Destructible>();
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Destructible);
	}
};

BOOST_CLASS_EXPORT_KEY(MonsterDestructible)
BOOST_CLASS_EXPORT_KEY(PlayerDestructible)
#endif /* DESTRUCTIBLE_HPP */
