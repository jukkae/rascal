#ifndef MAP_UTILS_HPP
#define MAP_UTILS_HPP
#include "../include/toml.hpp"

class Map;
class Actor;
class World;

namespace map_utils {
class ItemsTable { // TODO feels a bit iffy
public:
  static ItemsTable& getInstance() {
    static ItemsTable instance;
    return instance;
  }
  toml::table itemsTable;
private:
  ItemsTable() { itemsTable = toml::parse("assets/items.toml"); }
  ItemsTable(ItemsTable const&) = delete;
  void operator=(ItemsTable const&) = delete;
};

class BeingsTable {
public:
  static BeingsTable& getInstance() {
    static BeingsTable instance;
    return instance;
  }
  toml::table beingsTable;
private:
  BeingsTable() { beingsTable = toml::parse("assets/beings.toml"); }
  BeingsTable(BeingsTable const&) = delete;
  void operator=(BeingsTable const&) = delete;
};

class LevelsTable {
public:
  static LevelsTable& getInstance() {
    static LevelsTable instance;
    return instance;
  }
  toml::table levelsTable;
private:
  LevelsTable() { levelsTable = toml::parse("assets/levels.toml"); }
  LevelsTable(LevelsTable const&) = delete;
  void operator=(LevelsTable const&) = delete;
};

enum class TomlSource { BEINGS, ITEMS };

void addItems(World* world, Map* map, int difficulty = 1);
void addDoors(World* world, Map* map);
void addMonsters(World* world, Map* map, int difficulty = 1);
void addMonstersBasedOnRoomTypes(World* world, Map* map, int difficulty = 1);
void addItemsBasedOnRoomTypes(World* world, Map* map, int difficulty = 1);
void fixMainPath(World* world, Map* map, int difficulty = 1);
void addPlayer(World* world, Map* map);
void addStairs(World* world, Map* map,
               World* lower = nullptr,
               World* upper = nullptr);
std::unique_ptr<Actor> makeActorFromToml(World* world, Map* map, int x, int y, std::string type, TomlSource source);
} // namespace map_utils

#endif /* MAP_UTILS_HPP */
