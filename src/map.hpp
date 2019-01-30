#ifndef MAP_HPP
#define MAP_HPP
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <experimental/optional>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include "animation.hpp"
#include "constants.hpp"
#include "mat.hpp"
class World;
struct Rect;
template<class T>
struct Vec;

enum class Terrain { NORMAL, WATER };

struct Tile {
	bool explored;
	bool inFov;
	bool inEnemyFov; // TODO this belongs elsewhere
	bool transparent;
	bool walkable;
	Terrain terrain;
	float movementCost;
	std::experimental::optional<Animation> animation;
	Tile() : explored(false), inFov(false), inEnemyFov(false), transparent(true), walkable(true),
	terrain(Terrain::NORMAL), movementCost(1.0f) {;}

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & explored;
		// ar & inFov; no serialization for dynamic properties!
		ar & transparent;
		ar & walkable;
		ar & terrain;
		ar & movementCost;
		ar & animation;
	}
};


enum class BreakDirection { HORIZONTAL, VERTICAL };
enum class MapType { BUILDING, WATER, PILLARS };

class Map {
public:
	int width, height;
	//std::vector<Tile> tiles;
	Mat2d<Tile> tiles;
	bool hasAnimations = false;

	Map();
	Map(int width, int height, MapType mapType);

	void setWall(int x, int y);
	bool isWall(int x, int y) const;
	bool canWalk(int x, int y) const;
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	void setWorld(World* w) { world = w; }

private:
	World* world;

	void generateMap(MapType mapType = MapType::BUILDING);
	void generateBuildingMap();
	void generatePillarsMap();
	void generateWaterMap();
	std::vector<Rect> breakRooms(Rect area, BreakDirection direction = BreakDirection::HORIZONTAL);

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & width;
		ar & height;
		ar & tiles;
		ar & hasAnimations;
		ar & world;
	}
};
#endif /* MAP_HPP */
