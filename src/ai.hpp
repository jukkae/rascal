#ifndef AI_HPP
#define AI_HPP

class GameplayState;

#include "action.hpp"
#include "actor.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>

enum class Faction { PLAYER, ENEMY, NEUTRAL };

class Ai {
public:
	explicit Ai(float speed = 100, Faction faction = Faction::NEUTRAL) : speed(speed), faction(faction) {;}
	virtual ~Ai() {};
	float speed;
	bool isPlayer() { return faction == Faction::PLAYER; }
	virtual std::vector<std::unique_ptr<Action>> getNextAction(Actor* actor)
		{ std::vector<std::unique_ptr<Action>> as;
			as.insert(as.end(), std::make_unique<WaitAction>(WaitAction(actor)));
			return as; }
	Faction faction;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & speed;
		ar & faction;
	}
};

// For pathfinding
template<typename T, typename priority_t>
using PQElement = std::pair<priority_t, T>;

template<typename T, typename priority_t>
class PQElementComparator {
public:
int operator()(const PQElement<T, priority_t>& lhs, const PQElement<T, priority_t>& rhs)
	{
		return lhs.first > rhs.first; // Equivalent to std::greater
	}
};

template<typename T, typename priority_t>
struct PriorityQueue {
	using U = priority_t;

  std::priority_queue<PQElement<T, U>, std::vector<PQElement<T, U>>,
                 PQElementComparator<T, U>> elements;

  inline bool empty() const {
     return elements.empty();
  }

  inline void put(T item, priority_t priority) {
    elements.emplace(priority, item);
  }

  T get() {
    T best_item = elements.top().second;
    elements.pop();
    return best_item;
  }
};

class PlayerAi : public Ai {
public:
	PlayerAi() : Ai(100, Faction::PLAYER), xpLevel(1), experience(0) {;}

	int getNextLevelXp() const;
	int xpLevel;
	int experience;
	std::vector<std::unique_ptr<Action>> getNextAction(Actor* actor) override;
	void increaseXp(Actor* owner, int xp);
private:
	void handleActionKey(Actor* owner, int ascii, GameplayState* state);
	std::vector<Point> findPath(World* world, Point from, Point to);

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
	MonsterAi() : Ai(140, Faction::ENEMY), moveCount(0) {;}
	MonsterAi(float speed) : Ai(speed) {;}

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
