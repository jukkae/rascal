#ifndef EVENT_HPP
#define EVENT_HPP

#include "point.hpp"
#include "actor.hpp"
#include "status_effect.hpp"

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

struct ActionSuccessEvent : public Event {
	ActionSuccessEvent(Actor* actor = nullptr, std::string successMessage = ""): successMessage(successMessage) {;}

	Actor* actor;
	std::string successMessage; //TODO ultimately, again, this is GUI's responsibility
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

struct PlayerDeathEvent : public Event {
	PlayerDeathEvent(Actor* actor = nullptr): actor(actor) {;}

	Actor* actor;
};

struct PlayerStatChangeEvent : public Event {
	PlayerStatChangeEvent(Actor* actor = nullptr, bool levelUp = true, int xpLevel = 0): actor(actor), levelUp(levelUp), xpLevel(xpLevel) {;}

	Actor* actor;
	bool levelUp; //i know i know
	int xpLevel; //yeah bad
};

struct UiEvent : public Event { // This is, in effect, just a direct request to Gui::message()
	UiEvent(std::string text): text(text) {;}

	std::string text;
};

struct StatusEffectEvent : public Event {
	StatusEffectEvent(Actor* actor = nullptr, std::string text = ""): actor(actor), text(text) {;}

	Actor* actor;
	std::string text;
};

struct PickableHealthEffectEvent : public Event {
	PickableHealthEffectEvent(Actor* actor = nullptr, int amount = 0): actor(actor), amount(amount) {;}

	Actor* actor;
	int amount;
};

struct AiChangeEvent : public Event { // Yes I know this is getting ridiculous
	AiChangeEvent(Actor* actor = nullptr): actor(actor) {;}

	Actor* actor;
};

struct StatusEffectChangeEvent : public Event {
	StatusEffectChangeEvent(Actor* actor = nullptr, StatusEffect* effect = nullptr): actor(actor), effect(effect) {;}

	Actor* actor;
	StatusEffect* effect;
};

struct GenericActorEvent : public Event { // temp
	GenericActorEvent(Actor* actor = nullptr, std::string formatString = ""): actor(actor), formatString(formatString) {;}

	Actor* actor;
	std::string formatString;
};
#endif /* EVENT_HPP */
