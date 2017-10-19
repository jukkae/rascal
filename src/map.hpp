#ifndef MAP_HPP
#define MAP_HPP
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include "libtcod.hpp"
#include "vec.hpp"
class Actor;
class GameplayState;

struct Tile {
	bool explored;
	bool newInFov;
	Tile() : explored(false), newInFov(false) {;}

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & explored;
	}
};

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

class Map {
public:
	int width, height;
	std::vector<Tile> tiles;
	long seed;

	Map();
	Map(int width, int height);
	Map(int width, int height, long seed);
	~Map();
	void init(bool initActors);

	void setWall(int x, int y);
	void addMonster(int x, int y);
	void addItem(int x, int y);
	bool isWall(int x, int y) const;
	bool canWalk(int x, int y) const;
	bool isInFov(int x, int y) const;
	void markAsExplored(int x, int y);
	void markExploredTiles();
	bool isExplored(int x, int y) const;
	void computeFov();
	void computeFovForOctant(int x, int y, int octant);
	void setState(GameplayState* s) { state = s; }

protected:
	std::shared_ptr<TCODMap> map; // TODO shouldn't be shared_ptr, but unique can't be copied
	TCODRandom rng;
	friend class BspListener;

	void computeFovNew();
	bool isInFovNew(int x, int y);
	Vec<int> transformOctant(int row, int col, int octant);
	void dig(int x1, int y1, int x2, int y2);
	void createRoom(bool first, int x1, int y1, int x2, int y2, bool initActors);

private:
	GameplayState* state;
	Actor* makeMonster(int x, int y);
	Actor* makeItem(int x, int y);
	friend class boost::serialization::access;

	template<class Archive>
	friend void boost::serialization::save_construct_data(Archive & ar, const Map* m, const unsigned int file_version);

	template<class Archive>
	friend void boost::serialization::load_construct_data(Archive & ar, const Map* m, const unsigned int file_version);

	template<class Archive> // Not called anymore?
	void serialize(Archive & ar, const unsigned int version) {
		ar & width;
		ar & height;
		ar & tiles;
		ar & seed;
	}
};

namespace boost { namespace serialization {
template<class Archive>
	inline void save_construct_data (Archive & ar, const Map m, const unsigned int file_version) {
		ar << m.width;
		ar << m.height;
		ar << m.tiles;
		ar << m.seed;
	}

template<class Archive>
	inline void load_construct_data (Archive & ar, Map m, const unsigned int file_version) {
		int width;
		int height;
		std::vector<Tile> tiles;
		long seed;

		ar >> width;
		ar >> height;
		ar >> tiles;
		ar >> seed;

		::Map(width, height, seed);
		m.init(false);
	}
}} // namespace boost::serialization

class BspListener : public ITCODBspCallback {
private:
	int roomNum; // room number
	Map &map; // a map to dig
	int lastx, lasty; // center of the last room
	TCODRandom* rng;

public:
	BspListener(Map &map, TCODRandom* rng);
	bool visitNode(TCODBsp* node, void* userData);
};
#endif /* MAP_HPP */
