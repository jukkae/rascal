class Ai {
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
//BOOST_SERIALIZATION_ASSUME_ABSTRACT(Ai) TODO this won't work

class PlayerAi : public Ai {
public:
	void update(Actor* owner) override;

protected:
	bool moveOrAttack(Actor* owner, int targetX, int targetY);
	Actor* chooseFromInventory(Actor* owner);

private:
	void handleActionKey(Actor* owner, int ascii);

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		//ar.template register_type<Ai>();
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Ai);
	}
};

class MonsterAi : public Ai {
public:
	void update(Actor* owner) override;

protected:
	int moveCount;

	void moveOrAttack(Actor* owner, int targetX, int targetY);

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		//ar.template register_type<Ai>();
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Ai);
		ar & moveCount;
	}
};

class ConfusedMonsterAi : public Ai {
public:
	ConfusedMonsterAi(int turns, Ai* oldAi);
	ConfusedMonsterAi(); // TODO dirty hack
	void update(Actor* owner) override;

protected:
	int turns;
	Ai* oldAi;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		//ar.template register_type<Ai>();
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Ai);
		ar & turns;
		ar & oldAi;
	}
};

//BOOST_CLASS_EXPORT_KEY(PlayerAi)
//BOOST_CLASS_EXPORT_KEY(MonsterAi)
//BOOST_CLASS_EXPORT_KEY(ConfusedMonsterAi)
