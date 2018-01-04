#include "messaging.hpp"

#include "colors.hpp"
#include "gui.hpp" // TODO only included for Message struct -> move that here
#include "event.hpp"

Message messaging::createMessageFromEvent(Event& event) {
	if(auto e = dynamic_cast<ItemFoundEvent*>(&event)) {
		std::string messageText = e->getMessage().append((e->getItemName())); //TODO fix format string and line breaks: Messages should not care about line breaking
		return Message(messageText, colors::green);
	}
	else return Message(event.getMessage(), colors::white);
}
