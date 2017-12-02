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
