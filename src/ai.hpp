#ifndef AI_HPP
#define AI_HPP

class GameplayState;

#include <set>

#include "action.hpp"
#include "actor.hpp"
#include "direction.hpp"
#include "faction.hpp"

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
	std::set<Point> fov = {};
	void updateFov(Actor* actor);
	bool isInFov(int x, int y) { return fov.count(Point(x, y)) != 0; }
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

enum class AiState { NORMAL_WANDER, NORMAL_PATROL, NORMAL_IDLE, CURIOUS, SEEN_PLAYER_FRIENDLY, SEEN_PLAYER_HOSTILE };

class MonsterAi : public Ai {
public:
	MonsterAi() : Ai(140, factions::reavers), moveCount(0) {;}
	MonsterAi(float speed) : Ai(speed, factions::reavers) {;}
	MonsterAi(float speed, Faction& faction) : Ai(speed, faction) {;}

	std::vector<std::unique_ptr<Action>> getNextAction(Actor* actor) override;
	void setAiState(AiState state) {aiState = state;}
	void setPatrolPoints(std::vector<Point> points) {patrolPoints = points;}
	void setCurrentTargetIndex(int index);
	std::vector<Point> patrolPoints;
	Point* currentTarget = nullptr;
	int currentTargetIndex = 0;
protected:
	int moveCount;
	void moveOrAttack(Actor* owner, GameplayState* state, int targetX, int targetY);
private:
	AiState aiState = AiState::NORMAL_IDLE;
	bool hasSeenPlayer = false;
	std::vector<std::unique_ptr<Action>> plannedActions {};

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Ai);
		ar & moveCount;
		ar & aiState;
		ar & hasSeenPlayer;
		ar & patrolPoints;
		ar & currentTarget;
		ar & currentTargetIndex;
		//ar & plannedActions; // TODO this crashes on serialization, because action refers to actor, actor refers to AI, and... here we are
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
