#ifndef MAP_UTILS_HPP
#define MAP_UTILS_HPP

class Map;
class Actor;
class World;

namespace map_utils {
void addItems(World* world, Map* map, int difficulty = 1);
void addDoors(World* world, Map* map);
void addMonsters(World* world, Map* map, int difficulty = 1);
void addPlayer(World* world, Map* map);
void addStairs(World* world, Map* map);
void addStairs(World* world, Map* map, int dsX, int dsY); // TODO temp
void addMcGuffin(World* world, Map* map, int level);
} // namespace map_utils

namespace item { //TODO move to separate files
std::unique_ptr<Actor> makeItem(World* world, Map* map, int x, int y, int difficulty = 1);
std::unique_ptr<Actor> makeItemFromToml(World* world, Map* map, int x, int y, std::string type);
} // namespace item

namespace npc {
std::unique_ptr<Actor> makeMonster(World* world, Map* map, int x, int y, int difficulty = 1);
std::unique_ptr<Actor> makeBeingFromToml(World* world, Map* map, int x, int y, std::string type);
std::unique_ptr<Actor> makePunk(World* world, Map* map, int x, int y); //TODO move defs to data
std::unique_ptr<Actor> makeFighter(World* world, Map* map, int x, int y); //TODO move defs to data
std::unique_ptr<Actor> makeGuard(World* world, Map* map, int x, int y); //TODO move defs to data
std::unique_ptr<Actor> makeBoxer(World* world, Map* map, int x, int y); //TODO move defs to data
std::unique_ptr<Actor> makeMutant(World* world, Map* map, int x, int y); //TODO move defs to data
std::unique_ptr<Actor> makeCyborg(World* world, Map* map, int x, int y); //TODO move defs to data
std::unique_ptr<Actor> makeAndroid(World* world, Map* map, int x, int y); //TODO move defs to data
} // namespace npc

#endif /* MAP_UTILS_HPP */
