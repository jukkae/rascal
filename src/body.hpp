#ifndef BODY_HPP
#define BODY_HPP

#include "body_part.hpp"
#include <vector>

namespace boost { namespace serialization { class access; } }

enum class BodyType {
	BIOLOGICAL,
	MECHANICAL
};

class Body {
	//SPECIAL stats as a base, but with different range and bonuses, and speed added
public:
	Body(int s = 10, int p = 10, int e = 10, int c = 10, int i = 10, int a = 10, int l = 10, int spd = 10); //spd :D

	static Body createRandomBody();
	int getModifier(int attribute);

	std::vector<BodyPart> getFreeBodyParts();

	int strength;
	int perception;
	int endurance;
	int charisma;
	int intelligence;
	int agility;
	int luck;
	int speed;
	std::vector<std::pair<BodyPart, bool>> bodyParts; //true = available FIXME unclear
	BodyType bodyType = BodyType::BIOLOGICAL;
	//Skills, perks, traits, whatever
	//Radiation load
	int nutrition = 20000;
	int iodine = 0;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & strength;
		ar & perception;
		ar & endurance;
		ar & charisma;
		ar & intelligence;
		ar & agility;
		ar & luck;
		ar & speed;
		ar & bodyParts;
		ar & bodyType;
		ar & nutrition;
		ar & iodine;
    }
};
#endif /* BODY_HPP */
