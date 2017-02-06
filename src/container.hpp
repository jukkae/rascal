class Container {
public:
	int size; // max number of actors in container, 0 = unlimited - TODO think later about weight etc
	TCODList<Actor*> inventory;

	Container(int size);
	~Container();
	bool add(Actor* actor);
	void remove(Actor* actor);
};
