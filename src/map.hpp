struct Tile {
	bool canWalk;
	Tile() : canWalk(false) {;}
};

class Map {
public:
	int width, height;

	Map(int width, int height);
	~Map();
	void setWall(int x, int y);
	bool isWall(int x, int y) const;
	void render() const;

protected:
	Tile* tiles;
	friend class BspListener;

	void dig(int x1, int y1, int x2, int y2);
	void createRoom(bool first, int x1, int y1, int x2, int y2);
};

class BspListener : public ITCODBspCallback {
private:
	Map &map; // a map to dig
	int roomNum; // room number
	int lastx,lasty; // center of the last room

public:
	BspListener(Map &map);
	bool visitNode(TCODBsp *node, void *userData);
};
