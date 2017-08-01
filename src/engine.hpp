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

class Engine {
public:
	enum class GameStatus {
		STARTUP,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;
	std::vector<Actor*> actors;
	Actor* player; // TODO fix reliance on explicit pointer to player
	Actor* stairs;
	std::unique_ptr<Map> map;
	int fovRadius;
	int screenWidth;
	int screenHeight;
	Gui gui;
	Renderer renderer;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;
	int level;
	int time;

	Engine(int screenWidth, int screenHeight);
	~Engine();
	void init();
	void term();

	//void changeState(State* state);
	void pushState(State* state) { states.push_back(state); }
	void popState() { states.erase(states.end() - 1); }
	void update();
	void render();

	void nextLevel();
	void updateTime();

	Actor* getNextActor() const { return actors.at(0); }
	void updateNextActor();
	Actor* getPlayer() const;
	Actor* getClosestMonster(int x, int y, float range) const;
	Actor* getLiveActor(int x, int y) const;

	bool pickTile(int* x, int* y, float maxRange = 0.0f);
private:
	DummyState dummyState;
	std::vector<State*> states;

	friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & level;
		ar & time;
        ar & map;
		ar & stairs;
		ar & player;
		ar & actors;
		ar & gui;
    }
};

extern Engine engine; // TODO get rid of this, implement in a more robust way
#endif /* ENGINE_HPP */
