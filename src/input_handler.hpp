#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include "boost/optional.hpp"
// The point here being that "system" actions such as ESC is handled directly here,
// and PlayerAi *requests* keypresses and other events pertinent to itself from InputHandler
struct RawInputEvent {
	TCOD_key_t key;
	TCOD_mouse_t mouse;
	TCOD_event_t eventMask;
};

class Engine;
class InputHandler {
public:
	void handleEvents(Engine* engine);
	boost::optional<RawInputEvent> getEvent();
private:
	boost::optional<RawInputEvent> event;
};
#endif /* INPUT_HANDLER_HPP */
