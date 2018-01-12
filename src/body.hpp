#ifndef BODY_HPP
#define BODY_HPP

enum class BodyPart {
	HAND_L,
	HAND_R,
	FOOT_L,
	FOOT_R,
	TORSO,
	HEAD
};

enum class BodyType {
	BIOLOGICAL,
	MECHANICAL
};

class Body {
	//SPECIAL stats as a base, but with different range and bonuses
public:
	Body(int s = 10, int p = 10, int e = 10, int c = 10, int i = 10, int a = 10, int l = 10);

	static Body createRandomBody();
	int getModifier(int attribute);

	int strength;
	int perception;
	int endurance;
	int charisma;
	int intelligence;
	int agility;
	int luck;
	std::vector<BodyPart> bodyParts;
	BodyType bodyType = BodyType::BIOLOGICAL;

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
		ar & bodyParts;
		ar & bodyType;
    }
};
#endif /* BODY_HPP */
