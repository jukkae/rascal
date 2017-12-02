#include "world.hpp"
#include <iostream>

World::World(int width, int height): width(width), height(height) {
	map = Map(width, height);
}
