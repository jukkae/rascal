class Container {
public:
	int size; // max number of actors in container, 0 = unlimited
	std::vector<Actor*> inventory;

	Container(int size);
	Container(); // TODO dirty hack
	~Container();
	bool add(Actor* actor);
	void remove(Actor* actor);

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & size;
		ar & inventory;
	}
};
