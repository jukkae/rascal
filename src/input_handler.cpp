#include <iostream>
#include "libtcod.hpp"

#include "input_handler.hpp"

#include "engine.hpp"
#include "engine_command.hpp"
#include "main_menu_state.hpp"

void InputHandler::handleEvents(Engine* engine) {
	TCOD_key_t key;
	TCOD_mouse_t mouse;
	TCOD_event_t ev = TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &key, &mouse);
	if(ev == TCOD_EVENT_KEY_PRESS) {
		switch(key.vk) {
			case TCODK_ESCAPE: {
				State* mainMenuState = new MainMenuState();
				mainMenuState->init();
				engine->pushState(mainMenuState);
				return;
			}
			default: break;
		}
	}
	if(ev == TCOD_EVENT_MOUSE) {

	}
	event = (RawInputEvent) { key, mouse, ev };
}

boost::optional<RawInputEvent> InputHandler::getEvent() {
	boost::optional<RawInputEvent> ev = event;
	event = boost::none;
	return ev;
}
