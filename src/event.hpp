#ifndef EVENT_HPP
#define EVENT_HPP

#include "point.hpp"
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
	MeleeHitEvent(Actor* hitter = nullptr, Actor* hittee = nullptr, Actor* weapon = nullptr, int damage = 0, bool hit = false):
	hitter(hitter), hittee(hittee), weapon(weapon), damage(damage), hit(hit) {;}

	Actor* hitter;
	Actor* hittee; //:D
	Actor* weapon;
	int damage;
	bool hit;
};

struct RangedHitEvent : public Event {
	RangedHitEvent(Actor* hitter = nullptr, Actor* hittee = nullptr, Actor* weapon = nullptr, int damage = 0, bool hit = false):
	hitter(hitter), hittee(hittee), weapon(weapon), damage(damage), hit(hit) {;}

	Actor* hitter;
	Actor* hittee; //:D
	Actor* weapon;
	int damage;
	bool hit;
};

struct ActionFailureEvent : public Event {
	ActionFailureEvent(Actor* actor = nullptr, std::string failureMessage = ""): failureMessage(failureMessage) {;}

	Actor* actor;
	std::string failureMessage; //TODO ultimately, again, this is GUI's responsibility
};

struct RequestDescriptionEvent : public Event {
	RequestDescriptionEvent(Actor* actor = nullptr, Point location = Point(0, 0)): actor(actor), location(location) {;}

	Actor* actor;
	Point location;
	//TODO request description of a) surroundings and b) actors
};

struct DeathEvent : public Event {
	DeathEvent(Actor* actor = nullptr, std::string description = "", int xp = 0):
	actor(actor), description(description), xp(xp) {;}

	Actor* actor;
	std::string description;
	int xp;
};
#endif /* EVENT_HPP */
