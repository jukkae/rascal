#ifndef ENGINE_HPP
#define ENGINE_HPP
class Actor;
#include "renderer.hpp"
#include "gui.hpp"
#include "attacker.hpp"
#include "state.hpp"
#include "dummy_state.hpp"
#include "gameplay_state.hpp"
#include "persistent.hpp"

class Engine {
public:
	enum class GameStatus {
		STARTUP,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;
	std::shared_ptr<std::vector<Actor*>> actors = std::make_shared<std::vector<Actor*>>(); // moved
	Actor* player; // moved
	Actor* stairs; // moved
	Gui gui; // moved
	Renderer renderer; // moved
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;

	Engine(int dummy); // dummy parameter due to idiotic extern
	~Engine();
	void init();
	void term();

	//void changeState(State* state);
	void pushState(State* state) { states.push_back(state); }
	void popState() { states.pop_back(); }
	void update();
	void render();

	int getTime() { return gameplayState.getTime(); } // temporary
	void addActor(Actor* actor) { actors->push_back(actor); }
	void nextLevel();
	void updateTime();

	Actor* getNextActor() const { return actors->at(0); } // moved
	void updateNextActor(); // moved
	Actor* getPlayer() const { return gameplayState.getPlayer(); } // moved
	Actor* getClosestMonster(int x, int y, float range) const; // moved
	Actor* getLiveActor(int x, int y) const; // moved
	std::vector<Actor*>* getActors() { return actors.get(); }

	bool pickTile(int* x, int* y, float maxRange = 0.0f); // moved
private:
	DummyState dummyState;
	GameplayState gameplayState;
	std::vector<State*> states;

	friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & stairs;
		ar & player;
		ar & actors;
		ar & gui;
		ar & gameplayState; // TODO of course, serialize states
    }
};

extern Engine engine; // TODO get rid of this, implement in a more robust way
#endif /* ENGINE_HPP */
