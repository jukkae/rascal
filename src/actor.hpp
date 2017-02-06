class Actor {
public:
	int x, y;
	int ch; // ASCII code
	TCODColor col; // color
	const char* name;
	bool blocks; // does it block movement?
	Attacker* attacker;
	Destructible* destructible;
	Ai* ai; // self-updating
	Pickable* pickable;
	Container* container;

	Actor(int x, int y, int ch, const char* name, const TCODColor &col);
	~Actor();
	void update();
	void render() const;
};
