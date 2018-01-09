#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <string>
#include <queue>
#include <experimental/optional>
#include <boost/optional.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/vector.hpp>
#include <SFML/Graphics/Color.hpp>
class Action;
class Ai;
class Attacker;
class RangedAttacker;
class Destructible;
class Pickable;
class GameplayState;

#include "animation.hpp"
#include "container.hpp" // must be included for serialization
#include "destructible.hpp"
#include "pickable.hpp"
#include "status_effect.hpp" // does this have to be included?
#include "transporter.hpp"

enum class Statistic { CONSTITUTION, STRENGTH, AGILITY, SPEED };

class World;
class Actor {
public:
	int x, y; // FIXME reimplement in terms of Point
	int ch; // ASCII code // TODO
	sf::Color col; // color // TODO animation (?)
	std::string name;
	boost::optional<float> energy; // Shouldn't be public // TODO should be std::optional
	bool blocks; // does it block movement?
	bool fovOnly; // visible only when in fov?
	std::unique_ptr<Attacker> attacker;
	std::unique_ptr<RangedAttacker> rangedAttacker;
	std::unique_ptr<Destructible> destructible;
	std::unique_ptr<Ai> ai;
	std::unique_ptr<Pickable> pickable;
	std::unique_ptr<Container> container;
	std::unique_ptr<Transporter> transporter;
	Actor* wornWeapon = nullptr;
	std::experimental::optional<Animation> animation;

	GameplayState* s; // temporary for messaging TODO remove
	World* world = nullptr;

	Actor(int x = 0, int y = 0, int ch = 'x', std::string name = "", sf::Color col = sf::Color::White, boost::optional<float> energy = boost::none);
	~Actor();
	float update(GameplayState* state);
	float getDistance(int cx, int cy) const; // TODO doesn't belong here i suppose
	bool isPlayer();
	bool isStairs() { return transporter.get(); }
	void addAction(std::unique_ptr<Action> action) { actionsQueue.push_back(std::move(action)); }
	void setState(GameplayState* state) { s = state; } // temporary for getting access to state's actors TODO
	void modifyStatistic(Statistic stat, float delta);
	void addStatusEffect(std::unique_ptr<StatusEffect> statusEffect) { statusEffects.push_back(std::move(statusEffect)); }
	std::vector<std::unique_ptr<StatusEffect>>& getStatusEffects() { return statusEffects; }
	bool tryToMove(Direction direction, float distance);

private:
	std::deque<std::unique_ptr<Action>> actionsQueue;
	std::vector<std::unique_ptr<StatusEffect>> statusEffects;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & x;
		ar & y;
		ar & ch;
		ar & col;
		ar & name;
		ar & energy;
		ar & blocks;
		ar & fovOnly;
		ar & attacker;
		ar & rangedAttacker;
		ar & destructible;
		ar & ai;
		ar & pickable;
		ar & container;
		ar & actionsQueue;
		ar & transporter;
		ar & wornWeapon;
		ar & statusEffects;
		ar & world;
		ar & animation;
    }   
};
#endif /* ACTOR_HPP */
