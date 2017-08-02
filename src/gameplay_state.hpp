#ifndef GAMEPLAY_STATE_HPP
#define GAMEPLAY_STATE_HPP
#include "state.hpp"
class Actor;
class Engine;
class Map;
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "gui.hpp"
#include "renderer.hpp"

class GameplayState : public State {
public:
	void init() override;
	void cleanup() override;

	void update(Engine* engine) override;
	void render(Engine* engine) override;

	int getLevel() { return level; } // temporary!
	void increaseLevel() { ++level; } // temporary!
	int getTime() { return time; } // temporary!
	void increaseTime(int amount) { time += amount; } // temporary!
private:
	int screenWidth;
	int screenHeight;
	int fovRadius; // TODO doesn't really belong here
	int time = 0;
	int level = 1;
	std::vector<Actor*> actors; // TODO should be std::vector<std::unique_ptr<Actor>>
	Actor* player; // TODO fix reliance on explicitly pointing to player
	Actor* stairs; // likewise, this feels bad
	std::unique_ptr<Map> map;
	Gui gui;
	Renderer renderer;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) // TODO serialize ABC
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
#endif /* GAMEPLAY_STATE_HPP */
