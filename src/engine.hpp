#ifndef ENGINE_HPP
#define ENGINE_HPP
class Actor;
class Map;
#include "renderer.hpp"
#include "gui.hpp"
#include "attacker.hpp"
#include "map.hpp"
#include "scheduler.hpp"

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
	Scheduler scheduler;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;
	int level;
	float time;

	Engine(int screenWidth, int screenHeight);
	~Engine();
	void init();
	void term();
	void update();
	void render();
	void nextLevel();
	Actor* getPlayer() const;
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
		ar & gui;
		ar & scheduler;
    }
};

extern Engine engine; // TODO get rid of this, implement in a more robust way
#endif /* ENGINE_HPP */
