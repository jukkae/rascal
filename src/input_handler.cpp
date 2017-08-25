#include <iostream>
#include "libtcod.hpp"

#include "input_handler.hpp"

void InputHandler::handleEvents(Engine* engine) {
	TCOD_key_t key;
	TCOD_mouse_t mouse;
	TCOD_event_t event = TCODSystem::checkForEvent(TCOD_EVENT_ANY, &key, &mouse);
}
