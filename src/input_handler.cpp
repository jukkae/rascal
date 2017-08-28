#include <iostream>
#include "libtcod.hpp"

#include "input_handler.hpp"

#include "engine.hpp"
#include "engine_command.hpp"
#include "inventory_menu_state.hpp"
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
			case TCODK_CHAR: {
				if(key.c == 'i') {
					// TODO inventory needs to know about the player, so move elsewhere and pass info
					State* inventoryMenuState = new InventoryMenuState();
					inventoryMenuState->init();
					engine->pushState(inventoryMenuState);
					return;
				}
			}
			default: break;
		}
	}
	if(ev == TCOD_EVENT_MOUSE) {

	}
	event = (RawInputEvent) { key, mouse, ev };
}

boost::optional<RawInputEvent> InputHandler::getEvent() {
	// TODO change signature to getEvent(Actor* actor), call with this->owner from Ai
	// and then create new state if key.c == 'i'.
	// A bit nasty, but works without too much trouble.
	boost::optional<RawInputEvent> ev = event;
	event = boost::none;
	return ev;
}
