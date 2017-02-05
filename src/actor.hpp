class Actor {
public:
	int x, y;
	int ch; // ASCII code
	TCODColor col; // color

	Actor(int x, int y, int ch, const TCODColor &col);
	void render() const;
};
