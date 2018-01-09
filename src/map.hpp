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
class World;
struct Rect;
template<class T>
struct Vec;

enum class Terrain { NORMAL, WATER };

struct Tile {
	bool explored;
	bool inFov;
	bool transparent;
	bool walkable;
	Terrain terrain;
	std::experimental::optional<Animation> animation;
	Tile() : explored(false), inFov(false), transparent(true), walkable(true), terrain(Terrain::NORMAL) {;}

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & explored;
		// ar & inFov; no serialization for dynamic properties!
		ar & transparent;
		ar & walkable;
		ar & terrain;
		ar & animation;
	}
};

enum class FovType { SQUARE, CIRCLE };

class Shadow {
public:
	Shadow(float start, float end) : start(start), end(end) {;}
	float start;
	float end;

	static Shadow projectTile(int row, int column);
	bool contains(Shadow other);

};

class ShadowLine {
public:
	std::vector<Shadow> shadows;

	bool isInShadow(Shadow projection);
	bool isFullShadow() { return shadows.size() == 1 && shadows[0].start == 0 && shadows[0].end == 1; }
	void addShadow(Shadow shadow);
};

enum class BreakDirection { HORIZONTAL, VERTICAL };
enum class MapType { BUILDING, WATER, PILLARS };

class Map {
public:
	int width, height;
	std::vector<Tile> tiles;

	Map();
	Map(int width, int height);

	void generateMap(MapType mapType = MapType::BUILDING);
	void generateBuildingMap();
	void generatePillarsMap();
	void generateWaterMap();
	std::vector<Rect> breakRooms(Rect area, BreakDirection direction = BreakDirection::HORIZONTAL);

	void setWall(int x, int y);
	bool isWall(int x, int y) const;
	bool canWalk(int x, int y) const;
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	void computeFov(int x, int y, int radius = constants::DEFAULT_FOV_RADIUS, FovType fovType = FovType::CIRCLE);
	void computeFovForOctant(int x, int y, int octant, int radius = constants::DEFAULT_FOV_RADIUS, FovType fovType = FovType::CIRCLE);
	void setWorld(World* w) { world = w; }

private:
	World* world;
	Vec<int> transformOctant(int row, int col, int octant);
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & width;
		ar & height;
		ar & tiles;
		ar & world;
	}
};
#endif /* MAP_HPP */
