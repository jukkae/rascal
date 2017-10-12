#include <iostream>
#include "libtcod.hpp"

#include "input_handler.hpp"

#include "engine.hpp"
#include "engine_command.hpp"
#include "inventory_menu_state.hpp"
#include "level_up_menu_state.hpp"
#include "main_menu_state.hpp"

void InputHandler::handleEvents() {
	TCOD_key_t key;
	TCOD_mouse_t mouse;
	TCOD_event_t ev = TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &key, &mouse);
	if(ev == TCOD_EVENT_KEY_PRESS) {
		switch(key.vk) {
			case TCODK_ESCAPE: {
				State* mainMenuState = new MainMenuState();
				mainMenuState->init(engine);
				engine->pushState(mainMenuState);
				return;
			}
			case TCODK_CHAR: {

			}
			default: break;
		}
	}
	if(ev == TCOD_EVENT_MOUSE) {

	}
	event = (RawInputEvent) { key, mouse, ev };
}

boost::optional<RawInputEvent> InputHandler::getEvent(Actor* actor) {
	// TODO this is nasty, but inventory must know both the actor and engine
	if(event->key.c == 'i') {
		State* inventoryMenuState = new InventoryMenuState(actor);
		inventoryMenuState->init(engine);
		engine->pushState(inventoryMenuState);
		return boost::none; // TODO return empty RawInputEvent instead
	}
	if(event->key.c == 'l') { // TODO this is just for quick testing
		State* levelUpMenuState = new LevelUpMenuState(actor);
		levelUpMenuState->init(engine);
		engine->pushState(levelUpMenuState);
		return boost::none; // TODO return empty RawInputEvent instead
	}
	boost::optional<RawInputEvent> ev = event;
	event = boost::none;
	return ev;
}
