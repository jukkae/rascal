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
	std::string fmt;
	std::string messageText = "UNKNOWN EVENT";
	sf::Color color = colors::white;

	if(auto e = dynamic_cast<ItemFoundEvent*>(&event)) {
		fmt = "%d: There's a %s here!";
		messageText = formatString(fmt, e->time, e->item->name.c_str());
	}

	if(auto e = dynamic_cast<MeleeHitEvent*>(&event)) {
		if(!e->hit) {
			fmt = "%d: %s misses %s.";
			messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str());
		} else if(e->damage <= 0) {
			fmt = "%d: %s hits %s, but it seems to have no effect.";
			messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str());
		} else if(e->weapon) {
			fmt = "%d: %s attacks %s for %d hit points with a %s.";
			messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str(), e->damage, e->weapon->name.c_str());
		} else {
			fmt = "%d: %s attacks %s for %d hit points.";
			messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str(), e->damage);
		}
	}

	return Message(messageText, color);
}
