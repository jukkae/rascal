#ifndef ENGINE_HPP
#define ENGINE_HPP
class Actor;
class Map;
#include "renderer.hpp"
#include "gui.hpp"
#include "attacker.hpp"
#include "map.hpp"
#include "state.hpp"
#include "dummy_state.hpp"
#include "gameplay_state.hpp"

class Engine {
public:
	enum class GameStatus {
		STARTUP,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;
	std::vector<Actor*> actors; // moved
	Actor* player; // moved
	Actor* stairs; // moved
	std::unique_ptr<Map> map; // moved
	int screenWidth; // moved
	int screenHeight; // moved
	Gui gui; // moved
	Renderer renderer; // moved
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;

	Engine(int screenWidth, int screenHeight);
	~Engine();
	void init();
	void term();

	//void changeState(State* state);
	void pushState(State* state) { states.push_back(state); }
	void popState() { states.pop_back(); }
	void update();
	void render();

	int getLevel() { return gameplayState.getLevel(); } // temporary
	int getTime() { return gameplayState.getTime(); } // temporary
	void nextLevel();
	void updateTime();

	Actor* getNextActor() const { return actors.at(0); } // moved
	void updateNextActor(); // moved
	Actor* getPlayer() const; // moved
	Actor* getClosestMonster(int x, int y, float range) const; // moved
	Actor* getLiveActor(int x, int y) const; // moved

	bool pickTile(int* x, int* y, float maxRange = 0.0f); // moved
private:
	DummyState dummyState;
	GameplayState gameplayState;
	std::vector<State*> states;

	friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & map;
		ar & stairs;
		ar & player;
		ar & actors;
		ar & gui;
		ar & gameplayState; // TODO of course, serialize states
    }
};

extern Engine engine; // TODO get rid of this, implement in a more robust way
#endif /* ENGINE_HPP */
