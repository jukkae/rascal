#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP
// The point here being that "system" actions such as ESC is handled directly here,
// and PlayerAi *requests* keypresses and other events pertinent to itself from InputHandler
class Engine;
class InputHandler {
public:
	void handleEvents(Engine* engine);
};
#endif /* INPUT_HANDLER_HPP */
