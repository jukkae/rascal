class Ai {
public:
	virtual int update(Actor* owner) = 0;
	virtual ~Ai() {};
	static Ai* create (TCODZip& zip);
	int actionPoints = 0; // TODO crap
	int speed; // TODO crap
protected :
	enum class AiType {
		MONSTER, CONFUSED_MONSTER, PLAYER
	};
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {

	}
};

class PlayerAi : public Ai {
public:
	int xpLevel;
	PlayerAi();
	int getNextLevelXp();
	int update(Actor* owner) override;
	int actionPoints = 0;
	int speed = 100; // TODO crap

protected:
	bool moveOrAttack(Actor* owner, int targetX, int targetY);
	Actor* chooseFromInventory(Actor* owner);

private:
	void handleActionKey(Actor* owner, int ascii);

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Ai);
		ar & xpLevel;
	}
};

class MonsterAi : public Ai {
public:
	int update(Actor* owner) override;
	int actionPoints = 0;
	int speed = 200; // TODO crap

protected:
	int moveCount;

	void moveOrAttack(Actor* owner, int targetX, int targetY);

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Ai);
		ar & moveCount;
	}
};

class ConfusedMonsterAi : public Ai {
public:
	ConfusedMonsterAi(int turns, Ai* oldAi);
	ConfusedMonsterAi(); // TODO dirty hack
	int update(Actor* owner) override;

protected:
	int turns;
	Ai* oldAi;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Ai);
		ar & turns;
		ar & oldAi;
	}
};
