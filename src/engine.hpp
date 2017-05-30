class Engine {
public:
	enum GameStatus {
		STARTUP,
		IDLE,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;
	std::vector<Actor*> actors;
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
	void init();
	void save();
	void load();
	void update();
	void render();
	void sendToBack(Actor* actor);
	Actor* getClosestMonster(int x, int y, float range) const;
	Actor* getActor(int x, int y) const;
	bool pickTile(int* x, int* y, float maxRange = 0.0f);
private:
	friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // Simply list all the fields to be serialized/deserialized.
        ar & map; // works
		ar & player;
		ar & actors;
		ar & gui; // works?? or at least doesn't crash
    }
};

extern Engine engine;
