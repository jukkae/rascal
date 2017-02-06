class Engine {
public:
	enum GameStatus {
		STARTUP,
		IDLE,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;
	TCODList<Actor*> actors;
	Actor* player;
	Map* map;
	int fovRadius;
	int screenWidth;
	int screenHeight;
	Gui* gui;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;

	Engine(int screenWidth, int screenHeight);
	~Engine();
	void update();
	void render();
	void sendToBack(Actor* actor);
	Actor* getClosestMonster(int x, int y, float range) const;
};

extern Engine engine;
