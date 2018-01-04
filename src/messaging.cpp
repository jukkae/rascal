#include "messaging.hpp"

#include "colors.hpp"
#include "event.hpp"

std::string messaging::formatString(std::string text, ...) {
	va_list ap;
	std::string buf = "";
	char dest[1024*16]; // maybe something a bit more sane would be in order at some point
	va_start(ap, text);
	vsnprintf(dest, 1024*16, text.c_str(), ap); // FIXME
	va_end(ap);
	return std::string(dest);
	// std::istringstream iss (dest);
	// std::string line;
	// while (std::getline(iss, line, '\n')) {
	// 	// make room for the message
	// 	if(log.size() == MSG_HEIGHT) {
	// 		log.erase(log.begin());
	// 	}
	// 	Message msg(line, col);
	// 	log.push_back(msg);
	// } // multiline formatting for future reference
}


Message messaging::createMessageFromEvent(Event& event) {
	if(auto e = dynamic_cast<ItemFoundEvent*>(&event)) {
		//std::string messageText = e->getMessage().append((e->getItemName())); //TODO fix format string and line breaks: Messages should not care about line breaking

		std::string messageText = formatString(e->getMessage(), e->getItemName().c_str());
		return Message(messageText, colors::green);
	}
	else return Message(event.getMessage(), colors::white);
}
