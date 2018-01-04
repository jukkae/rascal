#include "messaging.hpp"

#include "actor.hpp"
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
		std::string fmt = "%d: There's a %s here!";
		std::string messageText = formatString(fmt, e->time, e->item->name.c_str());
		return Message(messageText, colors::green);
	}
	if(auto e = dynamic_cast<MeleeHitEvent*>(&event)) {
		if(e->damage <= 0) {
			std::string fmt = "%d: %s hits %s, but it seems to have no effect.";
			std::string messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str());
			return Message(messageText, colors::red);
		} else if(e->weapon) {
			std::string fmt = "%d: %s attacks %s for %d hit points with a %s.";
			std::string messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str(), e->damage, e->weapon->name.c_str());
			return Message(messageText, colors::red);
		} else {
			std::string fmt = "%d: %s attacks %s for %d hit points.";
			std::string messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str(), e->damage);
			return Message(messageText, colors::red);
		}
	}
	else return Message("UNKNOWN EVENT", colors::white);
}
