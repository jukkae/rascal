#ifndef MAP_UTILS_HPP
#define MAP_UTILS_HPP

class Map;
class Actor;
class World;

namespace map_utils {
void addItems(World* world, Map* map);
void addMonsters(World* world, Map* map, int difficulty = 1);
void addPlayer(World* world, Map* map);
void addStairs(World* world, Map* map);
void addStairs(World* world, Map* map, int dsX, int dsY); // TODO temp
void addMcGuffin(World* world, Map* map, int level);
} // namespace map_utils

namespace item { //TODO move to separate files
std::unique_ptr<Actor> makeItem(World* world, Map* map, int x, int y);
} // namespace item

namespace npc {
std::unique_ptr<Actor> makeMonster(World* world, Map* map, int x, int y, int difficulty = 1);
} // namespace npc

#endif /* MAP_UTILS_HPP */
