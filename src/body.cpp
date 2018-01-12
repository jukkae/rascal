#include "body.hpp"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <memory>
#include <vector>
#include "persistent.hpp"

Body::Body(int s, int p, int e, int c, int i, int a, int l):
strength(s),
perception(p),
endurance(e),
charisma(c),
intelligence(i),
agility(a),
luck(l)
{
	bodyParts.push_back(BodyPart::HAND_L);
	bodyParts.push_back(BodyPart::HAND_R);
	bodyParts.push_back(BodyPart::FOOT_L);
	bodyParts.push_back(BodyPart::FOOT_R);
	bodyParts.push_back(BodyPart::TORSO);
	bodyParts.push_back(BodyPart::HEAD);
};

Body::getModifier(int attribute) {
	if(attribute <= 3 ) return -2;
	if(attribute <= 7 ) return -1;
	if(attribute <= 13) return  0;
	if(attribute <= 17) return  1;
	if(attribute >= 18) return  2;
}
