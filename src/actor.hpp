#ifndef ACTOR_HPP
#define ACTOR_HPP

#include "libtcod.hpp"
#include <string>
#include <queue>
#include <boost/optional.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/vector.hpp>
class Attacker;
class Destructible;
class Pickable;
class GameplayState;

#include "ai.hpp"
#include "container.hpp" // must be included for serialization
#include "destructible.hpp"
#include "pickable.hpp"

class Actor {
public:
	int x, y;
	int ch; // ASCII code
	TCODColor col; // color
	std::string name;
	boost::optional<float> energy; // TODO proper encapsulation
	bool blocks; // does it block movement?
	bool fovOnly; // visible only when in fov?
	std::unique_ptr<Attacker> attacker;
	std::unique_ptr<Destructible> destructible;
	std::unique_ptr<Ai> ai;
	std::unique_ptr<Pickable> pickable;
	std::unique_ptr<Container> container;

	GameplayState* s; // temporary for messaging

	Actor(int x = 0, int y = 0, int ch = 'x', std::string name = "", const TCODColor& col = TCODColor::white, boost::optional<float> energy = boost::none);
	~Actor();
	float update(GameplayState* state);
	float getDistance(int cx, int cy) const;
	bool isPlayer() { return ai ? this->ai->isPlayer() : false; }
	std::vector<Actor*>* getActors(); // temporary for refactoring
	Actor* getPlayer(); // temporary for refactoring
	Actor* getClosestMonster(int x, int y, float range);
	Actor* getLiveActor(int x, int y);
	void setState(GameplayState* state) { s = state; } // temporary for getting access to state's actors

private:
	std::queue<Action*> actionsQueue;

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
		ar & destructible;
		ar & ai;
		ar & pickable;
		ar & container;
		// ar & actionsQueue; TODO serialization
    }   
};
#endif /* ACTOR_HPP */
