#ifndef EVENT_HPP
#define EVENT_HPP

class Actor;

struct Event {
	int time = 0; //set by world
	virtual ~Event() {}
};

struct ItemFoundEvent : public Event {
	ItemFoundEvent(Actor* finder = nullptr, Actor* item = nullptr):
	finder(finder), item(item) {;}

	Actor* finder;
	Actor* item;
};

struct MeleeHitEvent : public Event {
	MeleeHitEvent(Actor* hitter = nullptr, Actor* hittee = nullptr, Actor* weapon = nullptr, int damage = 0):
	hitter(hitter), hittee(hittee), weapon(weapon), damage(damage) {;}

	Actor* hitter;
	Actor* hittee; //:D
	Actor* weapon;
	int damage;
};
#endif /* EVENT_HPP */
