#include "body.hpp"

#include "dice.hpp"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <memory>
#include <vector>
#include "persistent.hpp"

Body::Body(int s, int p, int e, int c, int i, int a, int l, int spd):
strength(s),
perception(p),
endurance(e),
charisma(c),
intelligence(i),
agility(a),
luck(l),
speed(spd)
{
	bodyParts.push_back(std::pair<BodyPart, bool>(BodyPart::HAND_L, true));
	bodyParts.push_back(std::pair<BodyPart, bool>(BodyPart::HAND_R, true));
	bodyParts.push_back(std::pair<BodyPart, bool>(BodyPart::FEET,   true));
	bodyParts.push_back(std::pair<BodyPart, bool>(BodyPart::TORSO,  true));
	bodyParts.push_back(std::pair<BodyPart, bool>(BodyPart::HEAD,   true));
};

int Body::getModifier(int attribute) {
	if(attribute <= 3 ) return -2;
	if(attribute <= 7 ) return -1;
	if(attribute <= 13) return  0;
	if(attribute <= 17) return  1;
	if(attribute >= 18) return  2;
	return 0;
}

Body Body::createRandomBody() {
	int s = d6() + d6() + d6();
	int p = d6() + d6() + d6();
	int e = d6() + d6() + d6();
	int c = d6() + d6() + d6();
	int i = d6() + d6() + d6();
	int a = d6() + d6() + d6();
	int l = d6() + d6() + d6();
	int spd = 10; //default to normal speed
	return Body(s, p, e, c, i, a, l, spd);
}

std::vector<BodyPart> Body::getFreeBodyParts() {
	std::vector<BodyPart> freeParts;
	for(auto b : bodyParts) if(b.second) freeParts.push_back(b.first);
	return freeParts;
}
