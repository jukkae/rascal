class Ai : public Persistent {
public:
	virtual void update(Actor* owner) = 0;
	virtual ~Ai() {};
	static Ai* create (TCODZip& zip);
protected :
	enum AiType {
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
	void update(Actor* owner);

protected:
	bool moveOrAttack(Actor* owner, int targetX, int targetY);
	Actor* chooseFromInventory(Actor* owner);

private:
	void handleActionKey(Actor* owner, int ascii);

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	}
};

class MonsterAi : public Ai {
public:
	void update(Actor* owner);

protected:
	int moveCount;

	void moveOrAttack(Actor* owner, int targetX, int targetY);

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & moveCount;
	}
};

class ConfusedMonsterAi : public Ai {
public:
	ConfusedMonsterAi(int turns, Ai* oldAi);
	void update(Actor* owner);

protected:
	int turns;
	Ai* oldAi;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & turns;
		ar & oldAi;
	}
};
