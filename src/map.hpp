#ifndef MAP_HPP
#define MAP_HPP
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <optional>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include "animation.hpp"
#include "constants.hpp"
#include "faction.hpp"
#include "graph.hpp"
#include "mat.hpp"
#include "rect.hpp"
class World;

template<class T>
struct Vec;

enum class Terrain { NORMAL, WATER };

struct Tile {
	bool explored;
	bool transparent;
	bool walkable;
	Terrain terrain;
	float movementCost;
	std::optional<Animation> animation;
	Tile() : explored(false), transparent(true), walkable(true),
	terrain(Terrain::NORMAL), movementCost(1.0f) {;}

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & explored;
		ar & transparent;
		ar & walkable;
		ar & terrain;
		ar & movementCost;
		ar & animation;
	}
};


// Function of the room
enum class RoomType {
	UNASSIGNED,
	START,
	MARKET,
	NORMAL,
	COMMAND_CENTER,
	ARMOURY,
	HYDROPONICS,
	POWER_PLANT,
	WATER_PLANT,
	LIVING_QUARTERS,
	CLANDESTINE_LAB,
	VAULT,
	MEETING_ROOM,
	UPSTAIRS
};
// Special features of the room
enum class RoomDecor { NONE, PILLARS };

struct Room {
public:
	Rect coordinates;
	RoomType roomType;
	RoomDecor roomDecor;
	Faction roomFaction;
	int x0() { return coordinates.x0(); }
	int x1() { return coordinates.x1(); }
	int y0() { return coordinates.y0(); }
	int y1() { return coordinates.y1(); }
	int width() { return coordinates.width(); }
	int height() { return coordinates.height(); }

	// TODO not the most sophisticated, will break eventually
	bool operator==(const Room& rhs) const { return this->coordinates == rhs.coordinates; }
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & coordinates;
		ar & roomType;
		ar & roomDecor;
		ar & roomFaction;
	}
};


enum class BreakDirection { HORIZONTAL, VERTICAL };
enum class MapType { BUILDING, WATER, PILLARS };

class Map {
public:
	int width, height;
	//std::vector<Tile> tiles;
	Mat2d<Tile> tiles;
	Graph<Room> rooms; // Final adjacency map
	Graph<Room> minimumSpanningTree; // Backbone with exactly 1 route between every room
	Graph<Room> physicalConnectionsBetweenRooms; // All physical adjacencies
	std::vector<int> primaryPath {}; // Path from starting room to upstairs room
	bool hasAnimations = false;

	Map();
	Map(int width, int height, MapType mapType, World* world);

	void setWall(int x, int y);
	bool isWall(int x, int y) const;
	bool canWalk(int x, int y) const;
	//bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	std::vector<Room> getRooms(Point location);

private:
	World* world;

	void generateMap(MapType mapType = MapType::BUILDING);
	void generateBuildingMap();
	void generatePillarsMap();
	void generateWaterMap();
	Graph<Room> breakRooms(Rect area, BreakDirection direction = BreakDirection::HORIZONTAL);
	Graph<Room> indexRooms(Graph<Room> rooms);
	Graph<Room> connectRooms(Graph<Room> rooms);
	Graph<Room> pruneEdges(Graph<Room> rooms);
	Graph<Room> makeEdgesBidirectional(Graph<Room> rooms);
	Graph<Room> cullDoubleEdges(Graph<Room> rooms);
	Graph<Room> makeLoops(Graph<Room> rooms, const Graph<Room> physicalConnections, float loopFactor);
	Graph<Room> specializeRooms(Graph<Room> rooms, const Graph<Room> mst);

	std::vector<int> findPathBetweenRooms(Graph<Room> mst, int start, int end);

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & width;
		ar & height;
		ar & tiles;
		ar & rooms;
		ar & minimumSpanningTree;
		ar & physicalConnectionsBetweenRooms;
		ar & primaryPath;
		ar & hasAnimations;
		ar & world;
	}
};
#endif /* MAP_HPP */
