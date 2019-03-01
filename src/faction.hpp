#ifndef FACTION_HPP
#define FACTION_HPP

#include "colors.hpp"

class Faction {
public:
  std::string name;
  sf::Color color;
  int reputation = 0;
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & name;
    ar & color;
    ar & reputation;
  }
};

namespace factions {
static Faction player    { "player", colors::get("white") };
static Faction neutral   { "neutral", colors::get("white") };
static Faction police    { "police", colors::get("blue") };
static Faction reavers   { "reavers", colors::get("yellow") };
static Faction knives    { "the crimson knives", colors::get("red") };
static Faction syndicate { "the syndicate", colors::get("green") };
}; // namespace faction

#endif // FACTION_HPP
