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

	std::vector<Tile> tiles; // TODO these should be protected
	long seed; // and this too

	void setWall(int x, int y);
	void addMonster(int x, int y);
	void addItem(int x, int y);
	bool isWall(int x, int y) const;
	bool canWalk(int x, int y) const;
	bool isInFov(int x, int y); // TODO was marked as const
	bool isExplored(int x, int y) const;
	void computeFov();
	void render(); // TODO was marked as const

protected:
	TCODMap* map;
	TCODRandom* rng;
	friend class BspListener;

	void dig(int x1, int y1, int x2, int y2);
	void createRoom(bool first, int x1, int y1, int x2, int y2, bool initActors);

private:
	friend class boost::serialization::access;
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
		ar << m-> tiles;
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

		//TODO there's still something a bit off with saving and loading the random seed, idk what exactly
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
