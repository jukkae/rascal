#ifndef ENGINE_HPP
#define ENGINE_HPP
class Actor;
class Map;
#include "gui.hpp"

class Engine {
public:
	enum class GameStatus {
		STARTUP,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;
	std::vector<Actor*> actors;
	std::vector<std::pair<float, Actor*> > actorsQueue;
	Actor* player;
	Actor* stairs;
	std::unique_ptr<Map> map;
	int fovRadius;
	int screenWidth;
	int screenHeight;
	Gui gui;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;
	int level;

	Engine(int screenWidth, int screenHeight);
	~Engine();
	void init();
	void updateQueue();
	void term();
	void update();
	void render(); // TODO this should be const
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
        ar & map;
		ar & stairs;
		ar & player;
		ar & actors;
		ar & actorsQueue;
		ar & gui;
    }
};

extern Engine engine;
#endif /* ENGINE_HPP */
