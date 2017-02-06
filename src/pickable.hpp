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

