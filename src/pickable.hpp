class Pickable {
public:
	bool pick(Actor* owner, Actor* wearer);
	virtual bool use(Actor* owner, Actor* wearer);
	virtual ~Pickable() {};
	void drop(Actor* owner, Actor* wearer);
	virtual void save(TCODZip& zip) = 0;
	virtual void load(TCODZip& zip) = 0;
	static Pickable* create (TCODZip& zip);
protected:
	enum PickableType {
		HEALER, BLASTER_BOLT, FRAGMENTATION_GRENADE, CONFUSOR
	};
};

class Healer : public Pickable {
public:
	float amount;

	Healer(float amount);
	bool use(Actor* owner, Actor* wearer);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};

class BlasterBolt : public Pickable {
public:
	float range;
	float damage;
	BlasterBolt(float range, float damage);
	bool use(Actor* owner, Actor* wearer);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};

class FragmentationGrenade : public BlasterBolt { // TODO inherits from BB for fast dev
public:
	FragmentationGrenade(float range, float damage);
	bool use(Actor* owner, Actor* wearer);
	void save(TCODZip& zip);
	// void load(TCODZip& zip); can use BB load
};

class Confusor : public Pickable {
public:
	int turns;
	float range;
	Confusor(int turns, float range);
	bool use(Actor* owner, Actor* wearer);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};
