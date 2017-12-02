#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include "actor.hpp"
#include "map.hpp"

class GameplayState;
class World {
public:
	World(): World(120, 72) {;}
	World(int width, int height);

	void update();
	Actor* getNextActor() const { return actors.front().get(); }
	void updateNextActor();
	void updateTime();
	void sortActors();


	Actor* getPlayer() const;
	Actor* getStairs() const; // TODO up / down separately when we have persistent levels
	Actor* getClosestMonster(int x, int y, float range) const;
	Actor* getLiveActor(int x, int y) const;
	std::vector<std::unique_ptr<Actor>>& getActors() { return actors; }


	void computeFov() { map.computeFov(); }
	bool isWall(int x, int y) { return map.isWall(x, y); }
	bool canWalk(int x, int y);
	bool isInFov(int x, int y) { return map.isInFov(x, y); }

	void addActor(std::unique_ptr<Actor> actor) { actors.push_back(std::move(actor)); }



	int width;
	int height;
	int time = 0;
	int level = 1;
	std::vector<std::unique_ptr<Actor>> actors;
	Map map;
	GameplayState* state = nullptr;
private:
	friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
		ar & time;
		ar & level;
		ar & actors;
		ar & map;
		ar & state;
	}
};
#endif /* WORLD_HPP */
