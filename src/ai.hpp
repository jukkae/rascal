class Ai {
public:
	virtual int update(Actor* owner) = 0;
	virtual ~Ai() {};
	int speed; // TODO crap
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

class TemporaryAi : public Ai {
public:
	TemporaryAi(int turns = 0) : turns(turns) {;}
	int update(Actor* owner) override;
	void applyTo(Actor* actor);
protected:
	int turns;
	std::unique_ptr<Ai> oldAi;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Ai);
		ar & turns;
		ar & oldAi;
	}
};

class ConfusedMonsterAi : public TemporaryAi {
public:
	ConfusedMonsterAi(int turns = 0) : TemporaryAi(turns) {;}
	int update(Actor* owner) override;

protected:
	int turns;
	std::unique_ptr<Ai> oldAi;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(TemporaryAi);
		ar & turns;
		ar & oldAi;
	}
};
