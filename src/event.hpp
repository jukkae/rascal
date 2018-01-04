#ifndef EVENT_HPP
#define EVENT_HPP

class Actor;

struct Event {
	//TODO common required members:
	//- Actor / originator / sender: Who is this coming from? (may be NULL)
	//- Origin / position: Where did this happen? (may be NULL)
	//- Time: When did this happen? (Sort of redundant from messaging POV, but otherwise useful) (may not be NULL)
	//- Subject / target: Depends on *which* event precisely it was. (may definitely be NULL)
	virtual ~Event() {}
};

struct ItemFoundEvent : public Event {
	ItemFoundEvent(Actor* finder = nullptr, Actor* item = nullptr):
	finder(finder), item(item) {;}

	Actor* finder;
	Actor* item;
};
#endif /* EVENT_HPP */
