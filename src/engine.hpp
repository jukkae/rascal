#ifndef ENGINE_HPP
#define ENGINE_HPP
class Actor;
class Map;
#include "renderer.hpp"
#include "gui.hpp"
#include "attacker.hpp"
#include "map.hpp"

class Engine {
public:
	enum class GameStatus {
		STARTUP,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;
	std::vector<Actor*> actors;
	std::vector<std::pair<float, Actor*>> actorsQueue; // TODO maybe use std::set or map instead?
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
	float time;

	Engine(int screenWidth, int screenHeight);
	~Engine();
	void init();
	void updateQueue();
	void term();
	void update();
	void render(); // TODO this should be const
	void renderMap(const Map& map) const;
	void renderActor(const Actor& actor) const;
	void sendToBack(Actor* actor);
	void nextLevel();
	Actor* getClosestMonster(int x, int y, float range) const;
	Actor* getLiveActor(int x, int y) const;
	bool pickTile(int* x, int* y, float maxRange = 0.0f);
private:
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
		ar & actorsQueue;
		ar & gui;
    }
};

extern Engine engine; // TODO get rid of this, implement in a more robust way
#endif /* ENGINE_HPP */
