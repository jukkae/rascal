#ifndef EVENT_HPP
#define EVENT_HPP
class Event {
public:
	virtual std::string getMessage() { return "test\n"; }
};

class ItemFoundEvent : public Event {
public:
	std::string getMessage() override { return "item found test\n"; }
};
#endif /* EVENT_HPP */
