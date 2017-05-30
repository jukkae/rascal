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
	~Map();
	void init(bool initActors);

	void setWall(int x, int y);
	void addMonster(int x, int y);
	void addItem(int x, int y);
	bool isWall(int x, int y) const;
	bool canWalk(int x, int y) const;
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	void computeFov();
	void render() const;

protected:
	Tile* tiles;
	TCODMap* map;
	long seed;
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
