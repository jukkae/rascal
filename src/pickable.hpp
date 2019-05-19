#ifndef PICKABLE_HPP
#define PICKABLE_HPP

#include "openable.hpp"

class Actor;
class Effect;
class TargetSelector {
public:
	enum class SelectorType { CLOSEST_MONSTER, SELECTED_MONSTER, WEARER, WEARER_RANGE, SELECTED_RANGE, NONE };
	TargetSelector(SelectorType type = SelectorType::NONE, float range = 0);
	void selectTargets(Actor* wearer, std::vector<Actor*>& list);
	SelectorType type;
	float range;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & type;
		ar & range;
	}
};

class Pickable {
public:
	Pickable(TargetSelector selector = TargetSelector(TargetSelector::SelectorType::NONE, 0), std::unique_ptr<Effect> effect = std::unique_ptr<Effect>(), int weight = 1);
	virtual ~Pickable() {};
	bool pick(std::unique_ptr<Actor> owner, Actor* wearer);
	bool use (Actor* owner, Actor* wearer);
	bool hurl(Actor* owner, Actor* wearer); // Can't call this throw
	void drop(Actor* owner, Actor* wearer);
	void destroy(Actor* owner);
	int weight;
	bool fragile = false;
	bool explosive = false;
	LockType keyType = LockType::NONE;
protected:
	TargetSelector selector;
	std::unique_ptr<Effect> effect;
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
		ar & selector;
		ar & effect;
		ar & weight;
		ar & keyType;
    }
};

#endif /* PICKABLE_HPP */
