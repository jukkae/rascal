#ifndef MAP_UTILS_HPP
#define MAP_UTILS_HPP

class Map;
class Actor;
class World;

namespace map_utils {
void addItems(World* world, Map* map);
void addMonsters(World* world, Map* map);
void addPlayer(World* world, Map* map);
void addStairs(World* world, Map* map);
void addMcGuffin(World* world, Map* map, int level);

std::unique_ptr<Actor> makeMonster(World* world, Map* map, int x, int y);
std::unique_ptr<Actor> makeItem(World* world, Map* map, int x, int y);
void addMonster(World* world, Map* map, int x, int y);
void addItem(World* world, Map* map, int x, int y);
} // namespace map_utils

#endif /* MAP_UTILS_HPP */
