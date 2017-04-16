class Container : public Persistent {
public:
	int size; // max number of actors in container, 0 = unlimited - TODO think later about weight etc
	TCODList<Actor*> inventory;

	Container(int size);
	~Container();
	bool add(Actor* actor);
	void remove(Actor* actor);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};
