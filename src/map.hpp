#ifndef MAP_HPP
#define MAP_HPP
struct Tile {
	bool explored;
	Tile() : explored(false) {;}

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & explored;
	}
};

class Map {
public:
	int width, height;

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

protected:
	std::unique_ptr<TCODMap> map;
	TCODRandom rng;
	friend class BspListener;

	std::vector<Tile> tiles;
	long seed;

	void dig(int x1, int y1, int x2, int y2);
	void createRoom(bool first, int x1, int y1, int x2, int y2, bool initActors);

private:
	friend class boost::serialization::access;

	template<class Archive>
	friend void boost::serialization::save_construct_data(Archive & ar, const Map* m, const unsigned int file_version);

	template<class Archive>
	friend void boost::serialization::load_construct_data(Archive & ar, const Map* m, const unsigned int file_version);

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & width;
		ar & height;
		ar & tiles;
		ar & seed;
	}
};

namespace boost { namespace serialization {
template<class Archive>
	inline void save_construct_data (Archive & ar, const Map* m, const unsigned int file_version) {
		ar << m->width;
		ar << m->height;
		ar << m->tiles;
		ar << m->seed;
	}

template<class Archive>
	inline void load_construct_data (Archive & ar, Map* m, const unsigned int file_version) {
		int width;
		int height;
		std::vector<Tile> tiles;
		long seed;

		ar >> width;
		ar >> height;
		ar >> tiles;
		ar >> seed;

		::new(m)Map(width, height, seed);
		m->init(false);
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
