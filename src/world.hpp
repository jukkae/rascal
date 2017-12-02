#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include "actor.hpp"

class Map;
class World {
public:
	World();
	int time = 0;
	int level = 1;
	std::vector<std::unique_ptr<Actor>> actors;
	Map* map;
private:
	friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
		ar & time;
		ar & level;
		ar & actors;
		ar & map;
	}
};
#endif /* WORLD_HPP */
