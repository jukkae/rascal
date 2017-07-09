#ifndef AI_HPP
#define AI_HPP

class Actor;

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>

class Ai {
public:
	explicit Ai() : speed(100) {;}
	explicit Ai(float speed) : speed(speed) {;}
	virtual float update(Actor* owner) = 0;
	virtual ~Ai() {};
	float speed;
	virtual bool isPlayer() { return false; }
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & speed;
	}
};

class PlayerAi : public Ai {
public:
	PlayerAi() : xpLevel(1) {;}

	int getNextLevelXp() const;
	float update(Actor* owner) override;
	int xpLevel;
	bool isPlayer() override { return true; }
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
	MonsterAi() : Ai(140), moveCount(0) {;}
	MonsterAi(float speed) : Ai(speed) {;}
	float update(Actor* owner) override;
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
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(TemporaryAi);
	}
};
#endif /* AI_HPP */
