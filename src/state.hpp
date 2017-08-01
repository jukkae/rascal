#ifndef STATE_HPP
#define STATE_HPP
class Engine;
class State {
public:
	virtual void init() = 0;
	virtual void cleanup() = 0;

	// virtual void handleEvents(Engine* engine) = 0; TODO
	virtual void update(Engine* engine) = 0;
	virtual void render(Engine* engine) = 0;

	//void changeState(Engine* engine, State* state) { engine->changeState(state); } TODO not sure if necessary

// protected: State() { } TODO for implementing states as singletons
};
#endif /* STATE_HPP */
