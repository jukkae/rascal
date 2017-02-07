class Pickable {
public:
	bool pick(Actor* owner, Actor* wearer);
	virtual bool use(Actor* owner, Actor* wearer);
	virtual ~Pickable() {};
};

class Healer : public Pickable {
public:
	float amount;

	Healer(float amount);
	bool use(Actor* owner, Actor* wearer);
};

class BlasterBolt : public Pickable {
public:
	float range;
	float damage;
	BlasterBolt(float range, float damage);
	bool use(Actor* owner, Actor* wearer);
};

class FragmentationGrenade : public BlasterBolt { // TODO inherits from BB for fast dev
public:
	FragmentationGrenade(float range, float damage);
	bool use(Actor* owner, Actor* wearer);
};

class Confusor : public Pickable {
public:
	int turns;
	float range;
	Confusor(int turns, float range);
	bool use(Actor* owner, Actor* wearer);
};
