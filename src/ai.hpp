#ifndef AI_HPP
#define AI_HPP

class GameplayState;

#include "action.hpp"
#include "actor.hpp"
#include "direction.hpp"
#include "faction.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>

class Ai {
public:
	explicit Ai(float speed = 100, Faction& faction = factions::neutral) : speed(speed), faction(faction) {;}
	virtual ~Ai() {};
	float speed;
	bool isPlayer() { return faction.name == "player"; }
	virtual std::vector<std::unique_ptr<Action>> getNextAction(Actor* actor)
		{ std::vector<std::unique_ptr<Action>> as;
			as.insert(as.end(), std::make_unique<WaitAction>(WaitAction(actor)));
			return as; }
	Faction& faction;
	Direction currentDirection = Direction::NONE;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & speed;
		ar & faction;
		ar & currentDirection;
	}
};

class PlayerAi : public Ai {
public:
	PlayerAi() : Ai(100, factions::player), xpLevel(1), experience(0) {;}

	int getNextLevelXp() const;
	int xpLevel;
	int experience;
	std::vector<std::unique_ptr<Action>> getNextAction(Actor* actor) override;
	void increaseXp(Actor* owner, int xp);
private:
	void handleActionKey(Actor* owner, int ascii, GameplayState* state);

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Ai);
		ar & xpLevel;
		ar & experience;
	}
};

enum class AiState { NORMAL, FRIGHTENED, FRIENDLY };

class MonsterAi : public Ai {
public:
	MonsterAi() : Ai(140, factions::reavers), moveCount(0) {;}
	MonsterAi(float speed) : Ai(speed, factions::reavers) {;}
	MonsterAi(float speed, Faction& faction) : Ai(speed, faction) {;}

	std::vector<std::unique_ptr<Action>> getNextAction(Actor* actor) override;
protected:
	int moveCount;
	void moveOrAttack(Actor* owner, GameplayState* state, int targetX, int targetY);
private:
	AiState aiState = AiState::FRIENDLY;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Ai);
		ar & moveCount;
		ar & aiState;
	}
};

class TemporaryAi : public Ai {
public:
	TemporaryAi(int turns = 0) : turns(turns) {;}
	void applyTo(Actor* actor);

	virtual std::vector<std::unique_ptr<Action>> getNextAction(Actor* actor) override = 0;
	std::unique_ptr<Ai> oldAi; //TODO this should be private, but it's not for now
protected:
	int turns;

	void decreaseTurns(Actor* owner);
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

	std::vector<std::unique_ptr<Action>> getNextAction(Actor* actor) override;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(TemporaryAi);
	}
};
#endif /* AI_HPP */
