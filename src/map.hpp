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

class Map {
public:
	int width, height;
	std::vector<Tile> tiles;

	Map();
	Map(int width, int height);
	~Map();
	void init(bool initActors);

	void addItems(); // temp
	void addMonsters(); // temp

	void setWall(int x, int y);
	void addMonster(int x, int y);
	void addItem(int x, int y);
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
	}
};

namespace boost { namespace serialization {
template<class Archive>
	inline void save_construct_data (Archive & ar, const Map m, const unsigned int file_version) {
		ar << m.width;
		ar << m.height;
		ar << m.tiles;
	}

template<class Archive>
	inline void load_construct_data (Archive & ar, Map m, const unsigned int file_version) {
		int width;
		int height;
		std::vector<Tile> tiles;

		ar >> width;
		ar >> height;
		ar >> tiles;

		::Map(width, height);
		m.init(false);
	}
}} // namespace boost::serialization
#endif /* MAP_HPP */
