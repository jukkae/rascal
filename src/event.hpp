#ifndef EVENT_HPP
#define EVENT_HPP

struct Event {
	//TODO common required members:
	//- Actor / originator / sender: Who is this coming from? (may be NULL)
	//- Origin / position: Where did this happen? (may be NULL)
	//- Time: When did this happen? (Sort of redundant from messaging POV, but otherwise useful) (may not be NULL)
	//- Subject / target: Depends on *which* event precisely it was. (may definitely be NULL)
	virtual std::string getMessage() { return "test\n"; }
};

struct ItemFoundEvent : public Event {
	ItemFoundEvent(std::string message, std::string itemName):
	message(message), itemName(itemName) {;}

	std::string getMessage() override { return message; }
	std::string getItemName() { return itemName; }

	std::string message;
	std::string itemName;
};
#endif /* EVENT_HPP */
