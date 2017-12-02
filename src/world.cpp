#include "world.hpp"
#include <iostream>

World::World(int width, int height): width(width), height(height) {
	map = Map(width, height);
}

Actor* World::getPlayer() const {
	for(auto& actor : actors) {
        if(actor->isPlayer()) return actor.get();
    }
    return nullptr;
}

Actor* World::getStairs() const {
	for(auto& actor : actors) {
        if(actor->isStairs()) return actor.get();
    }
    return nullptr;
}

bool World::canWalk(int x, int y) {
	for(auto& actor : actors) {
		if(actor->blocks && actor->x == x && actor->y == y) {
			return false;
		}
	}
	return true;
}

