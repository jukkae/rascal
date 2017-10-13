#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include "boost/optional.hpp"
#include "point.hpp"
// The point here being that "system" actions such as ESC is handled directly here,
// and PlayerAi *requests* keypresses and other events pertinent to itself from InputHandler
struct RawInputEvent {
	TCOD_key_t key;
	TCOD_mouse_t mouse;
	TCOD_event_t eventMask;
};

class Engine;
class Actor;
class InputHandler {
public:
	InputHandler(Engine* engine) : engine(engine) {;}
	void handleEvents();
	boost::optional<RawInputEvent> getEvent(Actor* actor);
	Point getMouseLocation();
private:
	Point lastMouseLocation = Point(0, 0);
	Engine* engine;
	boost::optional<RawInputEvent> event;
};
#endif /* INPUT_HANDLER_HPP */
