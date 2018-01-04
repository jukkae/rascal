#ifndef EVENT_HPP
#define EVENT_HPP

struct Event {
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
