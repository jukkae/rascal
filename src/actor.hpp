#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <string>
#include <queue>
#include <optional>
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
class Armor;
class Attacker;
class Body;
class Container;
class RangedAttacker;
class Destructible;
class Pickable;
class StatusEffect;
class Transporter;
class Comestible;
class Wieldable;
class GameplayState;
class Mission;


#include "animation.hpp"
#include "attribute.hpp"
#include "direction.hpp"
#include "wieldable.hpp" //FIXME for serialization for now
#include "armor.hpp" //FIXME for serialization for now
#include "openable.hpp" //FIXME for serialization for now
#include "comestible.hpp" //FIXME for serialization for now
#include "transporter.hpp" //FIXME for serialization for now
#include "attacker.hpp" //FIXME for serialization for now
#include "body.hpp" //FIXME for serialization for now
#include "mission.hpp"
#include "dialogue_generator.hpp" //FIXME for serialization for now

class World;
class Actor {
public:
	int x, y; // FIXME reimplement in terms of Point
	int ch; // ASCII code // TODO
	sf::Color col; // color // TODO animation (?)
	std::string name;
	boost::optional<float> energy; // Shouldn't be public // TODO should be std::optional
	bool blocks; // does it block movement?
	bool blocksLight = false;
	bool fovOnly; // visible only when in fov?
	std::unique_ptr<Body> body;
	std::unique_ptr<Attacker> attacker;
	std::unique_ptr<RangedAttacker> rangedAttacker;
	std::unique_ptr<Armor> armor;
	std::unique_ptr<Destructible> destructible;
	std::unique_ptr<Ai> ai;
	std::unique_ptr<Pickable> pickable;
	std::unique_ptr<Container> container;
	std::unique_ptr<Transporter> transporter;
	std::unique_ptr<Comestible> comestible;
	std::unique_ptr<Wieldable> wieldable;
	std::unique_ptr<Openable> openable;
	Actor* wornWeapon = nullptr;
	std::vector<Actor*> wornArmors;
	std::optional<Animation> animation;
	std::vector<std::unique_ptr<Mission>> missions;
	std::unique_ptr<DialogueGenerator> dialogueGenerator;

	World* world;

	Actor(World* world = nullptr, int x = 0, int y = 0, int ch = 'x', std::string name = "", sf::Color col = sf::Color::White, boost::optional<float> energy = boost::none);
	~Actor();
	float update(GameplayState* state);
	float getDistance(int cx, int cy) const; // TODO doesn't belong here i suppose
	bool isPlayer();
	bool isStairs() { return transporter.get(); }
	void addAction(std::unique_ptr<Action> action) { actionsQueue.push_back(std::move(action)); }
	void modifyAttribute(Attribute attribute, int delta); // TODO move to body
	int getAttributeWithModifiers(Attribute attribute);
	void addStatusEffect(std::unique_ptr<StatusEffect> statusEffect) { statusEffects.push_back(std::move(statusEffect)); }
	void removeStatusEffect() { if(statusEffects.size() > 0) statusEffects.erase(statusEffects.begin()); } // TODO crap, remove by type
	std::vector<std::unique_ptr<StatusEffect>>& getStatusEffects() { return statusEffects; }
	bool tryToMove(Direction direction, float distance);
	int getAC();

	std::deque<std::unique_ptr<Action>> actionsQueue; // TODO this should be private, but visible to this actor's AI

private:

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
		ar & blocksLight;
		ar & fovOnly;
		ar & body;
		ar & wieldable;
		ar & openable;
		ar & attacker;
		ar & rangedAttacker;
		ar & armor;
		ar & destructible;
		ar & ai;
		ar & pickable;
		ar & container;
		ar & actionsQueue;
		ar & transporter;
		ar & wornWeapon;
		ar & wornArmors;
		ar & statusEffects;
		ar & world;
		ar & animation;
		ar & comestible;
		ar & missions;
		ar & dialogueGenerator;
    }
};
#endif /* ACTOR_HPP */
