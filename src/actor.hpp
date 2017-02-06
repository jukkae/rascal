class Actor {
public:
	int x, y;
	int ch; // ASCII code
	TCODColor col; // color
	const char* name;
	bool blocks; // does it block movement?
	Attacker* attacker; // something that can deal damage
	Destructible* destructible; // something that can be damaged
	Ai* ai; // something self-updating

	Actor(int x, int y, int ch, const char* name, const TCODColor &col);
	void update();
	void render() const;
};
