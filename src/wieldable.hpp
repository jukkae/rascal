#ifndef WIELDABLE_HPP
#define WIELDABLE_HPP

#include "body_part.hpp"

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
	WieldableType wieldableType = WieldableType::ANY;
private:
};

#endif /* WIELDABLE_HPP */
