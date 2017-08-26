#include <iostream>
#include "libtcod.hpp"

#include "input_handler.hpp"

void InputHandler::handleEvents(Engine* engine) {
	TCOD_key_t key;
	TCOD_mouse_t mouse;
	TCOD_event_t ev = TCODSystem::checkForEvent(TCOD_EVENT_ANY, &key, &mouse);
}

boost::optional<RawInputEvent> InputHandler::getEvent() {
	boost::optional<RawInputEvent> ev = event;
	event = boost::none;
	return ev;
}
