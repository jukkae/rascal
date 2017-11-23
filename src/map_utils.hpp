#ifndef MAP_UTILS_HPP
#define MAP_UTILS_HPP

class Map;
class GameplayState;
class Actor;

namespace map_utils {
void addItems(GameplayState* gameplayState, Map* map);
void addMonsters(GameplayState* gameplayState, Map* map);
void addPlayer(GameplayState* gameplayState, Map* map);
void addStairs(GameplayState* gameplayState, Map* map);

std::unique_ptr<Actor> makeMonster(GameplayState* gameplayState, Map* map, int x, int y);
std::unique_ptr<Actor> makeItem(GameplayState* gameplayState, Map* map, int x, int y);
void addMonster(GameplayState* gameplayState, Map* map, int x, int y);
void addItem(GameplayState* gameplayState, Map* map, int x, int y);
} // namespace map_utils

#endif /* MAP_UTILS_HPP */
