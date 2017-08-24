#ifndef AI_HPP
#define AI_HPP

class Actor;
class GameplayState;

#include "action.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>

enum class Faction { PLAYER, ENEMY, NEUTRAL };

class Ai {
public:
	explicit Ai(float speed = 100, Faction faction = Faction::NEUTRAL) : speed(speed), faction(faction) {;}
	virtual float update(Actor* owner, GameplayState* state) = 0;
	virtual ~Ai() {};
	float speed;
	bool isPlayer() { return faction == Faction::PLAYER; }
	virtual Action* getNextAction(Actor* actor) { return new EmptyAction(actor); }
protected:
	Faction faction;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & speed;
		ar & faction;
	}
};

class PlayerAi : public Ai {
public:
	PlayerAi() : Ai(100, Faction::PLAYER), xpLevel(1) {;}

	int getNextLevelXp() const;
	float update(Actor* owner, GameplayState* state) override;
	int xpLevel;
	Action* getNextAction(Actor* actor) override;
protected:
	bool moveOrAttack(Actor* owner, GameplayState* state, int targetX, int targetY);
	Actor* chooseFromInventory(Actor* owner);
private:
	void handleActionKey(Actor* owner, int ascii, GameplayState* state);

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Ai);
		ar & xpLevel;
	}
};

class MonsterAi : public Ai {
public:
	MonsterAi() : Ai(140, Faction::ENEMY), moveCount(0) {;}
	MonsterAi(float speed) : Ai(speed) {;}

	float update(Actor* owner, GameplayState* state) override;
protected:
	int moveCount;
	void moveOrAttack(Actor* owner, GameplayState* state, int targetX, int targetY);
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
	float update(Actor* owner, GameplayState* state) override;
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
	float update(Actor* owner, GameplayState* state) override;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(TemporaryAi);
	}
};
#endif /* AI_HPP */
