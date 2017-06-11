class Destructible {
public:
	float maxHp;
	float hp;
	float defense;
	std::string corpseName;
	
	Destructible(float maxHp = 0, float defense = 0, std::string corpseName = "corpse");
	virtual ~Destructible() {};

	inline bool isDead() { return hp <= 0; }
	float takeDamage(Actor* owner, float damage);
	float heal(float amount);
	virtual void die(Actor *owner);

	static Destructible* create(TCODZip& zip);
protected:
	enum DestructibleType {
		MONSTER, PLAYER
	};
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & maxHp;
		ar & hp;
		ar & defense;
		ar & corpseName;
	}
};

class MonsterDestructible : public Destructible {
public :
	MonsterDestructible(float maxHp = 0, float defense = 0, std::string corpseName = "corpse");
	void die(Actor *owner);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar.template register_type<Destructible>();
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Destructible);
	}
};

class PlayerDestructible : public Destructible {
public :
	PlayerDestructible(float maxHp = 0, float defense = 0, std::string corpseName = "your corpse");
	void die(Actor *owner);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar.template register_type<Destructible>();
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Destructible);
	}
};

BOOST_CLASS_EXPORT_KEY(MonsterDestructible)
BOOST_CLASS_EXPORT_KEY(PlayerDestructible)
