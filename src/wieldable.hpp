#ifndef WIELDABLE_HPP
#define WIELDABLE_HPP

#include "body_part.hpp"

namespace boost { namespace serialization { class access; } }

enum class WieldableType {
	ONE_HAND,
	TWO_HANDS,
	FEET,
	TORSO,
	HEAD,
	ANY
};

class Wieldable {
public:
	Wieldable(WieldableType wieldableType = WieldableType::ANY) : wieldableType(wieldableType) {;}
	WieldableType wieldableType;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & wieldableType;
    }
};

#endif /* WIELDABLE_HPP */
