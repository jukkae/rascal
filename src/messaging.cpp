#include "messaging.hpp"

#include "colors.hpp"
#include "gui.hpp"
#include "event.hpp"

Message messaging::createMessageFromEvent(Event& event) {
	if(auto e = dynamic_cast<ItemFoundEvent*>(&event)) {
		std::string messageText = e->getMessage().append((e->getItemName())); //TODO fix format string
		return Message(messageText, colors::green);
	}
	else return Message(event.getMessage(), colors::white);
}
