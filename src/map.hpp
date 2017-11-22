#ifndef MAP_HPP
#define MAP_HPP
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include "vec.hpp"
#include "rect.hpp"
class Actor;
class GameplayState;

struct Tile {
	bool explored;
	bool inFov;
	bool transparent;
	bool walkable;
	Tile() : explored(false), inFov(false), transparent(true), walkable(true) {;}

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & explored;
		// ar & inFov; no serialization for dynamic properties!
		ar & transparent;
		ar & walkable;
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
enum class MapType { BUILDING, PILLARS };

class Map {
public:
	int width, height;
	std::vector<Tile> tiles;

	Map();
	Map(int width, int height);
	~Map();

	void generateMap(MapType mapType = MapType::BUILDING);
	void generateBuildingMap();
	void generatePillarsMap();
	std::vector<Rect> breakRooms(Rect area, BreakDirection direction = BreakDirection::HORIZONTAL); // temp, s.b. private

	void setWall(int x, int y);
	bool isWall(int x, int y) const;
	bool canWalk(int x, int y) const;
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	void computeFov(FovType fovType = FovType::CIRCLE);
	void computeFovForOctant(int x, int y, int octant, FovType fovType = FovType::CIRCLE);
	void setState(GameplayState* s) { state = s; }

protected:
	Vec<int> transformOctant(int row, int col, int octant);
	void dig(int x1, int y1, int x2, int y2);
	void createRoom(bool first, int x1, int y1, int x2, int y2, bool initActors);

private:
	GameplayState* state;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & width;
		ar & height;
		ar & tiles;
	}
};
#endif /* MAP_HPP */
