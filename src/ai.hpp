#ifndef AI_HPP
#define AI_HPP
class Ai {
public:
	virtual float update(Actor* owner) = 0;
	virtual ~Ai() {};
	float speed;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & speed;
	}
};

class PlayerAi : public Ai {
public:
	int xpLevel;
	PlayerAi();
	int getNextLevelXp() const;
	float update(Actor* owner) override;
	float speed = 100;
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
	float update(Actor* owner) override;
	float speed = 140;
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
	float update(Actor* owner) override;
	void applyTo(Actor* actor);
	float speed = 140;
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
	float update(Actor* owner) override;
	float speed = 140;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(TemporaryAi);
	}
};
#endif /* AI_HPP */
