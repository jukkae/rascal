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

	Engine();
	~Engine();
	void update();
	void render();
private:
};

extern Engine engine;
