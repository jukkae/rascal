#ifndef ACTION_HPP
#define ACTION_HPP

class Actor;
class GameplayState;
struct ActionResult;

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/optional.hpp>

enum class Direction { N, NE, E, SE, S, SW, W, NW, NONE };

class Action {
public:
	Action(Actor* actor, float length = 100.0f) : actor(actor), length(length) {;}
	virtual bool execute() = 0;
	float getLength() { return length; }
protected:
	Actor* actor;
	float length;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & actor;
		ar & length;
	}
};

class EmptyAction : public Action {
public:
	EmptyAction(Actor* actor) : Action(actor, 100.0f) {;}
	bool execute() { return true; }
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
	}
};

class MoveAction : public Action {
public:
	MoveAction(Actor* actor, Direction direction) : Action(actor, 100.0f), direction(direction) {;}
	bool execute();
private:
	Direction direction;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
		ar & direction;
	}
};

class PickupAction : public Action {
public:
	PickupAction(Actor* actor) : Action(actor, 100.0f) {;}
	bool execute();
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
	}
};

struct ActionResult {
	bool succeeded;
	boost::optional<Action> alternativeAction;
};


#endif /* ACTION_HPP */
